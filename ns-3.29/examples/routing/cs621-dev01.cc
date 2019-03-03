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


// Network topology (CS621 project 1)
//
//      8Mbps               8Mbps
//  n0 ------- n1 ----- n2 -------n3
//
//


// (Original)
// Network topology
//
//  n0
//     \ 5 Mb/s, 2ms
//      \          1.5Mb/s, 10ms
//       n2 -------------------------n3
//      /
//     / 5 Mb/s, 2ms
//   n1
//
// - all links are point-to-point links with indicated one-way BW/delay
// - CBR/UDP flows from n0 to n3, and from n3 to n1
// - FTP/TCP flow from n0 to n3, starting at time 1.2 to time 1.35 sec.
// - UDP packet size of 210 bytes, with per-packet interval 0.00375 sec.
//   (i.e., DataRate of 448,000 bps)
// - DropTail queues
// - Tracing of queues and packet receptions to file "simple-global-routing.tr"

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
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
#if 0
  LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
#endif

  // Set up some default values for the simulation.  Use the
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));

  //DefaultValue::Bind ("DropTailQueue::m_maxPackets", 30);

  // Allow the user to override any of the defaults and the above
  // DefaultValue::Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
 // int t = 100; //fixed threshold t = 100 ms
  bool enableFlowMonitor = false;
  long compressionLinkCapacity = 0;
  cmd.AddNonOption("CompressionLinkCapacity", "Specify the maximum bandwidth", compressionLinkCapacity);
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.Parse (argc, argv);

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
  p2p.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("1500kbps"));
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
  //
  uint32_t packetSize = 2000;
  uint32_t maxPacketCount = 6000;

  Time interPacketInterval = Seconds (1.);
  RequestResponseClientHelper client (i2i3.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));

  apps = client.Install (c.Get (0));
 //
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));

  //create random data
  srand ( time(NULL) );
  uint8_t random_data[(int)packetSize];
  //int fd = popen("/dev/random", STA_RDONLY);
  //fread(fd, random_data, packetSize);
  //bytes_randomizer br;
  //std::random_device rd;
  //bytes_randomizer br(rd());
  //std::generate(std::begin(random_data), std::end(random_data), std::ref(br));
  for (int i=0; i<(int)packetSize-1; i++) {
    char c = (char)(random()&0x000000ff);
    random_data[i]= c;
    //cout << c << " ";
  }
  //cout << endl;
  //for (int i = 0; i<(int)packetSize-1;i++)
  //  cout<<hex<<setfill('0')<<setw(2) << random_data[i] <<" ";
  //cout <<endl;

  RequestResponseClientHelper client2 (i2i3.GetAddress (1), port);
  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client2.Install (c.Get (0));
  client2.SetFill(apps.Get((uint32_t)0), random_data, (uint32_t)packetSize);


  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));

  // // Create the OnOff application to send UDP datagrams of size
  // // 210 bytes at a rate of 448 Kb/s
  // NS_LOG_INFO ("Create Applications.");
  // uint16_t port = 9;   // Discard port (RFC 863)
  // OnOffHelper onoff ("ns3::UdpSocketFactory",
  //                    Address (InetSocketAddress (i2i3.GetAddress (1), port)));
  // onoff.SetConstantRate (DataRate ("448kb/s"));
  // ApplicationContainer apps = onoff.Install (c.Get (0));
  // apps.Start (Seconds (1.0));
  // apps.Stop (Seconds (5.0));

  // // Create a packet sink to receive these packets
  // PacketSinkHelper sink ("ns3::UdpSocketFactory",
  //                        Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  // apps = sink.Install (c.Get (3));
  // apps.Start (Seconds (1.0));
  // apps.Stop (Seconds (5.0));

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
    if (abs(first-second) > 100) {
      cout << "Compression Detected" << endl;
    }
    else {
      cout << "Compression Not Detected :(" << endl;
    }
  }
  /*(for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = container.begin();it!=container.end();++it)
  {
    //cout << it->first << endl;
  }*/
  NS_LOG_INFO ("Done.");

  Simulator::Destroy ();
  return 0;
}
