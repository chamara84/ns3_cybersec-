/*
 * rtds-Tap-ICS-Mod.cc
 *
 *  Created on: May 8, 2020
 *      Author: chamara
 *      Based on an example from the Univesity of Strathclyde
 */

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
 * along with this program; if not, write to the FreeEthernet (enp6s0f1) Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Use two hostonly interfaces in the virtual box
 * Run the setupInterface script
 * Add routes to the NS3 networks in windows
 * route ADD 192.168.10.0 MASK 255.255.255.0  192.168.19.4 METRIC 3 IF 3
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include "ns3/ipv4-address-generator.h"
#include <string>
#include <cassert>
#include <iomanip>
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/netanim-module.h"
#include "ns3/data-rate.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/point-to-point-module.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ns3/flow-monitor-helper.h"
#include "ns3/csma-helper.h"
 #include "ns3/queue.h"
 #include "ns3/drop-tail-queue.h"
#include "ns3/packet-metadata.h"
#include "ns3/attack-app.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/tap-bridge-helper.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("RTDS-Tap-Log");

int
main (int argc, char *argv[])
{


  //
  // We are interacting with the outside, real, world.  This means we have to
  // interact in real-time and therefore means we have to use the real-time
  // simulator and take the time to calculate checksums.
  //
  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


  // Create ns3 nodes for simulating the communications network
  Ptr<Node> n0 = CreateObject<Node> ();
  Ptr<Node> n1 = CreateObject<Node> ();
  Ptr<Node> n2 = CreateObject<Node> ();
  Ptr<Node> n3 = CreateObject<Node> ();
  Ptr<Node> n4 = CreateObject<Node> ();




  NodeContainer n0n1n2n3 = NodeContainer(n0,n1,n2,n3,n4);

  CsmaHelper csmaNetwork;
  csmaNetwork.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csmaNetwork.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560))); // be very carefull about the delay that is applied to CSMA link, excessive delays will stack due to half duplex behaviour of the link


  // end of building the network



     NetDeviceContainer dn0n1n2n3 = csmaNetwork.Install (n0n1n2n3); //Network access to the outside



// end of building the network

// installing IP stacks into the nodes
InternetStackHelper stack;
stack.Install (n0);
stack.Install (n1);
stack.Install (n2);
stack.Install (n3);
stack.Install (n4);






// For interfaces between real linux interface and interface between simulation node

// *** NOTE: because initialised first CSMA i.e. the interface to real dices is INDEX = 1

Ipv4AddressHelper ipv4;




ipv4.SetBase ("10.103.40.0", "255.255.254.0","0.0.1.117"); //this is the outer network
Ipv4InterfaceContainer ipn0n1n2n3 = ipv4.Assign (dn0n1n2n3);




// Get access to the IPv4 stack of the nodes to push routing tables
 Ptr<Ipv4> ipv4n0 = n0->GetObject<Ipv4> ();
 Ptr<Ipv4> ipv4n1 = n1->GetObject<Ipv4> ();
 Ptr<Ipv4> ipv4n2 = n2->GetObject<Ipv4> ();
 Ptr<Ipv4> ipv4n3 = n3->GetObject<Ipv4> ();
 Ptr<Ipv4> ipv4n4 = n4->GetObject<Ipv4> ();


// Setup routing

//Automatic routing setup  does not work, cant see end device IP addresses

Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//can use manual routing too (complete-untested)



// Use the TapBridgeHelper to connect to the pre-configured tap devices for
// the left side.  We go with "UseBridge" mode since the CSMA devices support
// promiscuous mode and can therefore make it appear that the bridge is
// extended into ns-3.  The install method essentially bridges the specified
// tap to the specified CSMA device.
//
// Connect the left side tap to the left side CSMA device in ghost node n0

  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap00"));
  tapBridge.Install (n0, dn0n1n2n3.Get (0));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap01"));
  tapBridge.Install (n4, dn0n1n2n3.Get (4));

//
// Connect the right side tap to the right side CSMA device in ghost node n4
//







          //setup application


                                uint32_t attackerId = 3;
                                uint32_t attackerId2 = 2;


                                Address victimAddr;

  csmaNetwork.EnablePcapAll ("pmuconnectiontestNet", false);


                                std::pair<Ptr<Ipv4>, uint32_t> returnValue = ipn0n1n2n3.Get (attackerId);

                                Ptr<Ipv4> ipv4Val = returnValue.first;
                                uint32_t index = returnValue.second;

                                Ptr<Ipv4Interface> iface =  ipv4Val->GetObject<Ipv4L3Protocol> ()->GetInterface (index);


                                Ptr<AttackApp> attacker = CreateObject<AttackApp> ();
                                std::vector<Ipv4Address> spoofedIPs{Ipv4Address ("10.103.41.207"),};
                                                                std::vector<Ipv4Address>victimIPs{Ipv4Address ("10.103.41.161"),Ipv4Address ("10.103.41.164"),Ipv4Address ("10.103.41.165"),Ipv4Address ("10.103.41.166")};
                                                                std::vector<Address>victimMACs{ns3::Mac48Address("60:15:92:10:1b:cb"),ns3::Mac48Address("60:15:92:10:1b:cb"),ns3::Mac48Address("60:15:92:10:1b:cb"),ns3::Mac48Address("60:15:92:10:1b:cb")};

                               attacker->Setup(n0n1n2n3.Get(attackerId), dn0n1n2n3.Get(attackerId), iface, spoofedIPs, victimIPs, victimMACs);
                                n0n1n2n3.Get (attackerId)->AddApplication (attacker);
                                attacker->SetStartTime (Seconds (1.0));
                                attacker->SetStopTime (Seconds (3600.0));



                                std::pair<Ptr<Ipv4>, uint32_t> returnValue2 = ipn0n1n2n3.Get (attackerId);

                                                                Ptr<Ipv4> ipv4Val2 = returnValue2.first;
                                                                uint32_t index2= returnValue2.second;

                                                                Ptr<Ipv4Interface> iface2 =  ipv4Val2->GetObject<Ipv4L3Protocol> ()->GetInterface (index2);


                                                                Ptr<AttackApp> attacker2 = CreateObject<AttackApp> ();
                                                                std::vector<Ipv4Address> spoofedIPs1{Ipv4Address ("10.103.41.161"),Ipv4Address ("10.103.41.164"),Ipv4Address ("10.103.41.165"),Ipv4Address ("10.103.41.166")};
                                                                std::vector<Ipv4Address>victimIPs1{Ipv4Address ("10.103.41.207"),Ipv4Address ("10.103.41.207"),Ipv4Address ("10.103.41.207"),Ipv4Address ("10.103.41.207")};
                                                                std::vector<Address>victimMACs1{ns3::Mac48Address("10:65:30:f5:c9:a5"),ns3::Mac48Address("10:65:30:f5:c9:a5"),ns3::Mac48Address("10:65:30:f5:c9:a5"),ns3::Mac48Address("10:65:30:f5:c9:a5")};

                                                               attacker2->Setup(n0n1n2n3.Get(attackerId), dn0n1n2n3.Get(attackerId), iface,  spoofedIPs1, victimIPs1,victimMACs1);
                                                                n0n1n2n3.Get (attackerId)->AddApplication (attacker2);
                                                                attacker2->SetStartTime (Seconds (1.0));
                                                                attacker2->SetStopTime (Seconds (3600.0));





csmaNetwork.EnablePcapAll ("pmuconnectiontestNet", false);


  // Run the simulation for ten minutes to give the user time to play around
  //
  Simulator::Stop (Seconds (3600.));
  Simulator::Run ();
  printf("Running Simulation \n");
  Simulator::Destroy ();
}
