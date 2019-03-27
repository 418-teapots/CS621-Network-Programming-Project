/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
//Example commandline: ./waf --run "cs621-dev01 --cap=1 --comp=1"

// Network topology (CS621 project 1)
//
//      8Mbps               8Mbps
//  n0 ------- n1 ----- n2 -------n3
//

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include <climits>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <map>
// #include "zlib.h"


#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/event-id.h"
#include "ns3/histogram.h"
#include "ns3/flow-classifier.h"
#include "ns3/flow-probe.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/flow-monitor.h"
//#include "ns3/packet.h"
using std::cout;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimpleGlobalRoutingExample");

int
main (int argc, char *argv[])
{
  //declare  of given variables

  uint32_t packetSize; //#size of the packets
  uint32_t maxPacketCount;//# of packets to send
  std::string outerDataRate; //link capacity for the two outer links
  int threshold; //fixed threshold t = 100 ms
  int compressionLinkCapacity; //link capacity for the inner link
  bool useCompression; //use compression or not

  //initailze variables with values from config file
  std::ifstream cFile("config/config.txt");
  if (cFile.is_open())
  {
    cout << "Config file detected" << endl;
    std::string line;
    while(getline(cFile, line))
    {
      line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
      auto delimiter = line.find("=");
      auto name = line.substr(0, delimiter);
      auto value = line.substr(delimiter + 1);
      if (name == "packetSize") {
        packetSize = (uint32_t)stoi(value);
        //cout << "Packet Size set to " + std::__cxx11::to_string((int)packetSize) << endl;
      }
      else if (name == "maxPacketCount") {
        maxPacketCount = (uint32_t)stoi(value);
        //cout << "Max Packet Count set to " + std::__cxx11::to_string((int)maxPacketCount) << endl;
      }
      else if (name == "outerDataRate") {
        outerDataRate = value;
        //cout << "Outer Data Rate set to " + outerDataRate << endl;
      }
      else if (name == "threshold") {
        threshold = stoi(value);
        //cout << "Threshold set to " + std::__cxx11::to_string(threshold) << endl;
      }
    }
  }
  else {
    cout << "No config file detected" << endl;
    return 0;
  }



  // Allow the user to override any of the defaults and the above
  CommandLine cmd;
  cmd.AddValue("cap", "Specify the maximum bandwidth", compressionLinkCapacity);
  cmd.AddValue("comp", "Use compression?", useCompression);
  cmd.Parse (argc, argv);
  if (compressionLinkCapacity == 0) {
    cout << "You must specify the compression link capacity!" << endl; // must specify compression link capacity
    return 0;
  }
  std::string innerDataRate = std::to_string(compressionLinkCapacity) + "Mbps"; //set the inner link capcity


  //Create four nodes and form a group
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (4);
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));

  //install internet
  InternetStackHelper internet;
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue (outerDataRate));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1); //outer links
  NetDeviceContainer d2d3 = p2p.Install (n2n3); //outer links

  p2p.SetDeviceAttribute ("DataRate", StringValue (innerDataRate));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer d1d2 = p2p.Install (n1n2); //inner link
  Ptr<NetDevice> compressionDevice = d1d2.Get(0);
  Ptr<PointToPointNetDevice> p2pCompDevice = StaticCast<PointToPointNetDevice>(compressionDevice);
  Ptr<NetDevice> decompressionDevice = d1d2.Get(1);
  Ptr<PointToPointNetDevice> p2pDecompDevice = StaticCast<PointToPointNetDevice>(decompressionDevice);
  p2pCompDevice->SetCompressionFlag(useCompression);
  p2pDecompDevice->SetDecompressionFlag(useCompression);


  //Add IP addresses
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  // Create router nodes, initialize routing database and set up the routing tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // (Server)
  // Create a RequestResponseServer application on node three.

  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;  // well-known echo port number
  RequestResponseServerHelper server (port);
  // uint32_t responseSize = 1024;
  // server.SetAttribute ("PacketSize", UintegerValue (responseSize));
  ApplicationContainer apps = server.Install (c.Get (3));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (2000.0));

  // (Client)
  // Create a RequestResponseClient application to send UDP datagrams from node zero to node three.

  //The first client will send packet train with empty data (all zeroes)

  Time interPacketInterval = Seconds (0.1);
  RequestResponseClientHelper client (i2i3.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client.Install (c.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (2000.0));



  //The second client will send packet train with random data (high entropy)

  RequestResponseClientHelper client2 (i2i3.GetAddress (1), port);
  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client2.Install (c.Get (0));
  client2.SetFill(apps.Get((uint32_t)0), true , (uint32_t)packetSize); //Call this function to make high entropy data
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (2000.0));

  //generate pcap files
  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream ("cs621-dev01.tr"));
  p2p.EnablePcap("UDPsender.pcap",d0d1.Get(0), false, true);
  p2p.EnablePcap("UDPreceiver.pcap",d2d3.Get(1), false, true);
  p2p.EnablePcap("Compression.pcap",d1d2.Get(0), false, true);
  p2p.EnablePcap("Decompression.pcap",d1d2.Get(1), false, true);
  //p2p.EnablePcapAll ("cs621-dev01");

  //Flow Monitor
  //Use flow monitor to get stats on when the last packet in the packet train arrives
  FlowMonitorHelper flowmonHelper;
  Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll ();
  NS_LOG_INFO ("Run Simulation.");
  //Simulator::Schedule(Seconds(0.2),&sendHandler,udp, nodes2, Ptr<Packet>(&a));
  Simulator::Stop (Seconds (2000));
  Simulator::Run ();
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  int64_t first = 0; //save time for first packet train
  int64_t second = 0; //save time for second packet train
  int count = 0; //count how many packet trains were detected
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
      if (t.sourceAddress == Ipv4Address("10.1.1.1") && t.destinationAddress == Ipv4Address("10.1.3.2"))
      {
        FlowMonitor::FlowStats fs = iter->second;
        if (first == 0)
          first = fs.timeLastRxPacket.GetMilliSeconds() - fs.timeFirstRxPacket.GetMilliSeconds();
        else
          second = fs.timeLastRxPacket.GetMilliSeconds() - fs.timeFirstRxPacket.GetMilliSeconds();
        count ++;
      }

    }

  //if the flow monitor detected more than two trains, there is something wrong
  if (count > 2)
  {
    cout << "more than two trains sent" << endl;
  }
  else {
    if (abs(first-second) > threshold) { //if the difference between the two trains is bigger than the threshhold
      cout << std::__cxx11::to_string((int) first) << " and " << std::__cxx11::to_string((int) second) << " Compression Detected" << endl;
    }
    else {
      cout << std::__cxx11::to_string((int) first) << " and " << std::__cxx11::to_string((int) second) << " Compression Not Detected :(" << endl;
    }
  }
  NS_LOG_INFO ("Done.");

  Simulator::Destroy ();
  return 0;
}
