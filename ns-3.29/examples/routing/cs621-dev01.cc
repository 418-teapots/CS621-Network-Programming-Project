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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
//#include "ns3/packet.h"

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
  uint32_t packetSize = 1100;
  uint32_t maxPacketCount = 6000;

  Time interPacketInterval = Seconds (1.);
  RequestResponseClientHelper client (i2i3.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  
  apps = client.Install (c.Get (0));
  client.SetFill(apps, (uint8_t)0, (uint32_t)1100);
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));
  RequestResponseClientHelper client2 (i2i3.GetAddress (1), port);
  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (100));
  apps = client2.Install (c.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));
  //srandom(getpid());
 // for (i=0; i<MAX_PACK_SZ-1; i++)
  //random_data[i]=(char)(random()&0x000000ff);
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
  if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }

  NS_LOG_INFO ("Run Simulation.");
  //Simulator::Schedule(Seconds(0.2),&sendHandler,udp, nodes2, Ptr<Packet>(&a));
  Simulator::Stop (Seconds (20));
  Simulator::Run ();
  
  NS_LOG_INFO ("Done.");

  if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("cs621-dev01.flowmon", false, false);
    }

  Simulator::Destroy ();
  return 0;
}
