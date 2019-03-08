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
//Example commandline: ./waf --run "cs621-dev01 --cap=1"

// Network topology (CS621 project 1)
//
//      8Mbps               8Mbps
//  n0 ------- n1 ----- n2 -------n3
//

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <map>
#include <zlib.h>

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
  // Allow the user to override any of the defaults and the above
  // DefaultValue::Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
  uint32_t packetSize = 1100; //#size of the packets
  uint32_t maxPacketCount = 1; //# of packets to send
  std::string outerDataRate = "8Mbps";
  int threshold = 100; //fixed threshold t = 100 ms
  int compressionLinkCapacity = 0;
  bool useCompression = true;
  cmd.AddValue("cap", "Specify the maximum bandwidth", compressionLinkCapacity);
  cmd.AddValue("comp", "Use compression?", useCompression);
  cmd.Parse (argc, argv);
  if (compressionLinkCapacity == 0) {
    cout << "You must specify the compression link capacity!" << endl;
    return 0;
  }
  std::string innerDataRate = std::to_string(compressionLinkCapacity) + "Mbps";
  // Here, we will explicitly create four nodes.  In more sophisticated
  // topologies, we could configure a node factory.
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (4);
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));

  InternetStackHelper internet;
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue (outerDataRate));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);

  p2p.SetDeviceAttribute ("DataRate", StringValue (innerDataRate));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer d1d2 = p2p.Install (n1n2);

  // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // (Server)
  // Create a RequestResponseServer application on node three.
  //
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;  // well-known echo port number
  RequestResponseServerHelper server (port);
  // uint32_t responseSize = 1024;
  // server.SetAttribute ("PacketSize", UintegerValue (responseSize));
  ApplicationContainer apps = server.Install (c.Get (3));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (20.0));

  // (Client)
  // Create a RequestResponseClient application to send UDP datagrams from node zero to
  // node three.


  //Time interPacketInterval = Seconds (1.);
  RequestResponseClientHelper client (i2i3.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  //client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));

  apps = client.Install (c.Get (0));
 //
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));

  //create random data for high entropy
  srand ( time(NULL) );
  uint8_t random_data[(int)packetSize];
  for (int i=0; i<(int)packetSize-1; i++) {
    char c = (char)(random()&0x000000ff);
    random_data[i]= c;
  }

  RequestResponseClientHelper client2 (i2i3.GetAddress (1), port);
  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
//client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client2.Install (c.Get (0));
  client2.SetFill(apps.Get((uint32_t)0), random_data, (uint32_t)packetSize);
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));

  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream ("cs621-dev01.tr"));
  p2p.EnablePcapAll ("cs621-dev01");

  // Flow Monitor
  FlowMonitorHelper flowmonHelper;
  Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll ();
  NS_LOG_INFO ("Run Simulation.");
  //Simulator::Schedule(Seconds(0.2),&sendHandler,udp, nodes2, Ptr<Packet>(&a));
  Simulator::Stop (Seconds (20));
  Simulator::Run ();
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  int64_t first = 0;
  int64_t second = 0;
  int count = 0;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
      if (t.sourceAddress == Ipv4Address("10.1.1.1") && t.destinationAddress == Ipv4Address("10.1.3.2"))
      {
        FlowMonitor::FlowStats fs = iter->second;
        if (first == 0)
          first = fs.timeLastRxPacket.GetMilliSeconds();
        else
          second = fs.timeLastRxPacket.GetMilliSeconds();
        count ++;
      }

    }
  if (count > 2)
  {
    cout << "more than two trains sent" << endl;
  }
  else {
    if (abs(first-second) > threshold) {
      cout << "Compression Detected" << endl;
    }
    else {
      cout << "Compression Not Detected :(" << endl;
    }
  }
  NS_LOG_INFO ("Done.");

  Simulator::Destroy ();
  return 0;
}
