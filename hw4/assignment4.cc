#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RipSimpleRouting");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
	nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
	nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}

int main (int argc, char **argv)
{
	/*------------------------------------------------- Setup command line args ----------------------------------*/
	bool printRoutingTables = false;
	bool showPings = false;

	CommandLine cmd (__FILE__);
	cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
	cmd.AddValue ("printRoutingTables", "Print routing tables at 10, 40 and 80 seconds", printRoutingTables);
	cmd.Parse (argc, argv);
	
	/*------------------------------------------------- Setup split horizon with poisin reverse -------------------*/
	Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));

	/*------------------------------------------------- Create hosts and routers ---------------------------------*/
	NS_LOG_INFO ("Create nodes.");
	
	Ptr<Node> a = CreateObject<Node> ();
	Names::Add ("HostA", a);
	Ptr<Node> b = CreateObject<Node> ();
	Names::Add ("HostB", b);
	Ptr<Node> c = CreateObject<Node> ();
	Names::Add ("HostC", c);
	Ptr<Node> d = CreateObject<Node> ();
	Names::Add ("HostD", d);
	Ptr<Node> e = CreateObject<Node> ();
	Names::Add ("HostE", e);
	Ptr<Node> f = CreateObject<Node> ();
	Names::Add ("HostF", f);
	Ptr<Node> g = CreateObject<Node> ();
	Names::Add ("HostG", g);
	Ptr<Node> h = CreateObject<Node> ();
	Names::Add ("HostH", h);
	Ptr<Node> i = CreateObject<Node> ();
	Names::Add ("HostI", i);
	Ptr<Node> j = CreateObject<Node> ();
	Names::Add ("HostJ", j);
	Ptr<Node> k = CreateObject<Node> ();
	Names::Add ("HostK", k);
	
	Ptr<Node> r1 = CreateObject<Node> ();
	Names::Add ("Router1", r1);
	Ptr<Node> r2 = CreateObject<Node> ();
	Names::Add ("Router2", r2);
	Ptr<Node> r3 = CreateObject<Node> ();
	Names::Add ("Router3", r3);
	Ptr<Node> r4 = CreateObject<Node> ();
	Names::Add ("Router4", r4);
	Ptr<Node> r5 = CreateObject<Node> ();
	Names::Add ("Router5", r5);
	Ptr<Node> r6 = CreateObject<Node> ();
	Names::Add ("Router6", r6);
	
	/*------------------------------------------------- Connect routers and hosts ----------------------------------*/
	NodeContainer net1 (r1, a);		//1
	NodeContainer net2 (r1, b);		//2
	NodeContainer net3 (r1, c);		//3
	NodeContainer net4 (r1, r3);	//4

	NodeContainer net5 (r2, d);		//1
	NodeContainer net6 (r2, e);		//2
	NodeContainer net7 (r2, f);		//3
	NodeContainer net8 (r2, g);		//4
	NodeContainer net9 (r2, r4);	//5

	NodeContainer net10 (r3, r4);	//2
	NodeContainer net11 (r3, r5);	//3

	NodeContainer net12 (r4, r5);	//3
	NodeContainer net13 (r4, r6);	//4

	NodeContainer net14 (r5, h);	//3
	NodeContainer net15 (r5, i);	//4

	NodeContainer net16 (r6, j);	//2
	NodeContainer net17 (r6, k);	//3

	NodeContainer routers1 (r1, r2, r3);
	NodeContainer routers2 (r4, r5, r6);
	NodeContainer nodes1 (a ,b, c);
	NodeContainer nodes2 (d, e, f, g);
	NodeContainer nodes3 (h, i);
	NodeContainer nodes4 (j, k);

	/*------------------------------------------------- Create links ----------------------------------*/
	NS_LOG_INFO ("Create channels.");
	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
	csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
	NetDeviceContainer ndc1 = csma.Install (net1);
	NetDeviceContainer ndc2 = csma.Install (net2);
	NetDeviceContainer ndc3 = csma.Install (net3);
	NetDeviceContainer ndc4 = csma.Install (net4);
	NetDeviceContainer ndc5 = csma.Install (net5);
	NetDeviceContainer ndc6 = csma.Install (net6);
	NetDeviceContainer ndc7 = csma.Install (net7);
	NetDeviceContainer ndc8 = csma.Install (net8);
	NetDeviceContainer ndc9 = csma.Install (net9);
	NetDeviceContainer ndc10 = csma.Install (net10);
	NetDeviceContainer ndc11 = csma.Install (net11);
	NetDeviceContainer ndc12 = csma.Install (net12);
	NetDeviceContainer ndc13 = csma.Install (net13);
	NetDeviceContainer ndc14 = csma.Install (net14);
	NetDeviceContainer ndc15 = csma.Install (net15);
	NetDeviceContainer ndc16 = csma.Install (net16);
	NetDeviceContainer ndc17 = csma.Install (net17);

	/*------------------------------------------------- Assign RIP metrics and remove hosts from RIP ----------------------------------*/
	NS_LOG_INFO ("Create IPv4 and routing");
	RipHelper ripRouting;

	ripRouting.ExcludeInterface (r1, 1);	// r1->a
	ripRouting.ExcludeInterface (r1, 2);	// r1->b
	ripRouting.ExcludeInterface (r1, 3);	// r1->c
	
	ripRouting.ExcludeInterface (r2, 1);	// r2->d
	ripRouting.ExcludeInterface (r2, 2);	// r2->e
	ripRouting.ExcludeInterface (r2, 3);	// r2->f
	ripRouting.ExcludeInterface (r2, 4);	// r2->g

	ripRouting.ExcludeInterface (r5, 3);	// r5->h
	ripRouting.ExcludeInterface (r5, 4);	// r5->i

	ripRouting.ExcludeInterface (r6, 2);	// r6->j
	ripRouting.ExcludeInterface (r6, 3);	// r6->k

	ripRouting.SetInterfaceMetric (r4, 3, 5);
	ripRouting.SetInterfaceMetric (r5, 2, 5);

	/*------------------------------------------------- Setup IPv4 ------------------------------------------*/
	Ipv4ListRoutingHelper listRH;
	listRH.Add (ripRouting, 0);

	InternetStackHelper internet;
	internet.SetIpv6StackInstall (false);
	internet.SetRoutingHelper (listRH);
	internet.Install (routers1);
	internet.Install (routers2);

	InternetStackHelper internetNodes;
	internetNodes.SetIpv6StackInstall (false);
	internetNodes.Install (nodes1);
	internetNodes.Install (nodes2);
	internetNodes.Install (nodes3);
	internetNodes.Install (nodes4);

	/*------------------------------------------------- Assign IPv4 addresses ----------------------------------*/
	NS_LOG_INFO ("Assign IPv4 Addresses.");
	Ipv4AddressHelper ipv4;

	ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);

	ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);

	ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);

	ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);

	ipv4.SetBase (Ipv4Address ("10.0.4.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);

	ipv4.SetBase (Ipv4Address ("10.0.5.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);

	ipv4.SetBase (Ipv4Address ("10.0.6.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);

	ipv4.SetBase (Ipv4Address ("10.0.7.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic8 = ipv4.Assign (ndc8);

	ipv4.SetBase (Ipv4Address ("10.0.8.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic9 = ipv4.Assign (ndc9);

	ipv4.SetBase (Ipv4Address ("10.0.9.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic10 = ipv4.Assign (ndc10);

	ipv4.SetBase (Ipv4Address ("10.0.10.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic11 = ipv4.Assign (ndc11);

	ipv4.SetBase (Ipv4Address ("10.0.11.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic12 = ipv4.Assign (ndc12);

	ipv4.SetBase (Ipv4Address ("10.0.12.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic13 = ipv4.Assign (ndc13);

	ipv4.SetBase (Ipv4Address ("10.0.13.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic14 = ipv4.Assign (ndc14);

	ipv4.SetBase (Ipv4Address ("10.0.14.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic15 = ipv4.Assign (ndc15);

	ipv4.SetBase (Ipv4Address ("10.0.15.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic16 = ipv4.Assign (ndc16);

	ipv4.SetBase (Ipv4Address ("10.0.16.0"), Ipv4Mask ("255.255.255.0"));
	Ipv4InterfaceContainer iic17 = ipv4.Assign (ndc17);

	/*------------------------------------------------- Setup static host IP addresses ----------------------------------*/
	Ptr<Ipv4StaticRouting> staticRouting;
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (a->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.0.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (b->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.1.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (c->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.2.1", 1 );
	
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (d->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.4.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (e->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.5.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (f->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.6.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (g->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.7.1", 1 );
	
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (h->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.13.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (i->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.14.1", 1 );
	
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (j->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.15.1", 1 );
	staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (k->GetObject<Ipv4> ()->GetRoutingProtocol ());
	staticRouting->SetDefaultRoute ("10.0.16.1", 1 );

	/*------------------------------------------------- Print routing tables ----------------------------------*/
	if (printRoutingTables)
	{
		RipHelper routingHelper;

		Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);

		routingHelper.PrintRoutingTableAt (Seconds (10.0), r1, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (10.0), r2, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (10.0), r3, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (10.0), r4, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (10.0), r5, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (10.0), r6, routingStream);

		routingHelper.PrintRoutingTableAt (Seconds (40.0), r1, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (40.0), r2, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (40.0), r3, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (40.0), r4, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (40.0), r5, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (40.0), r6, routingStream);

		routingHelper.PrintRoutingTableAt (Seconds (80.0), r1, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (80.0), r2, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (80.0), r3, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (80.0), r4, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (80.0), r5, routingStream);
		routingHelper.PrintRoutingTableAt (Seconds (80.0), r6, routingStream);
	}

	/*------------------------------------------------- Create pings ----------------------------------*/
	NS_LOG_INFO ("Create Applications.");
	uint32_t packetSize = 1024;
	Time interPacketInterval = Seconds (1.0);
	
	V4PingHelper ping1 ("10.0.16.2");
	ping1.SetAttribute ("Interval", TimeValue (interPacketInterval));
	ping1.SetAttribute ("Size", UintegerValue (packetSize));
	ping1.SetAttribute ("Verbose", BooleanValue (showPings));
	ApplicationContainer apps1 = ping1.Install (a);
	apps1.Start (Seconds (1.0));
	apps1.Stop (Seconds (81.0));	

	V4PingHelper ping2 ("10.0.13.2");
	ping2.SetAttribute ("Interval", TimeValue (interPacketInterval));
	ping2.SetAttribute ("Size", UintegerValue (packetSize));
	ping2.SetAttribute ("Verbose", BooleanValue (showPings));
	ApplicationContainer apps2 = ping2.Install (g);
	apps2.Start (Seconds (2.0));
	apps2.Stop (Seconds (92.0));

	/*------------------------------------------------- Export PCAP ----------------------------------*/
	csma.EnablePcapAll ("assignment-4", true);

	/*------------------------------------------------- Break link ----------------------------------*/
	Simulator::Schedule (Seconds (25), &TearDownLink, r3, r4, 2, 2);


	/*------------------------------------------------- Create netAnim xml ----------------------------------*/
	 
	MobilityHelper mobility;
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.Install (routers1);
	mobility.Install (routers2);
	mobility.Install (nodes1);
	mobility.Install (nodes2);
	mobility.Install (nodes3);
	mobility.Install (nodes4);

	AnimationInterface anim("animation.xml");

	Ptr<ConstantPositionMobilityModel> r1Anim = routers1.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> r2Anim = routers1.Get (1)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> r3Anim = routers1.Get (2)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> r4Anim = routers2.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> r5Anim = routers2.Get (1)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> r6Anim = routers2.Get (2)->GetObject<ConstantPositionMobilityModel> ();

	Ptr<ConstantPositionMobilityModel> naAnim = nodes1.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> nbAnim = nodes1.Get (1)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> ncAnim = nodes1.Get (2)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> ndAnim = nodes2.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> neAnim = nodes2.Get (1)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> nfAnim = nodes2.Get (2)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> ngAnim = nodes2.Get (3)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> nhAnim = nodes3.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> niAnim = nodes3.Get (1)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> njAnim = nodes4.Get (0)->GetObject<ConstantPositionMobilityModel> ();
	Ptr<ConstantPositionMobilityModel> nkAnim = nodes4.Get (1)->GetObject<ConstantPositionMobilityModel> ();


	r1Anim->SetPosition (Vector ( 2.0, 4.0, 0  ));
	r2Anim->SetPosition (Vector ( 13.0, 4.0, 0  ));
	r3Anim->SetPosition (Vector ( 6.0, 8.0, 0  ));
	r4Anim->SetPosition (Vector ( 9.0, 8.0, 0  ));
	r5Anim->SetPosition (Vector ( 6.0, 12.0, 0  ));
	r6Anim->SetPosition (Vector ( 9.0, 12.0, 0  ));
	
	naAnim->SetPosition (Vector ( 0.0, 1.0, 0  ));
	nbAnim->SetPosition (Vector ( 2.0, 1.0, 0  ));
	ncAnim->SetPosition (Vector ( 4.0, 1.0, 0  ));
	ndAnim->SetPosition (Vector ( 10.0, 1.0, 0  ));
	neAnim->SetPosition (Vector ( 12.0, 1.0, 0  ));
	nfAnim->SetPosition (Vector ( 14.0, 1.0, 0  ));
	ngAnim->SetPosition (Vector ( 16.0, 1.0, 0  ));
	nhAnim->SetPosition (Vector ( 5.0, 15.0, 0  ));
	niAnim->SetPosition (Vector ( 7.0, 15.0, 0  ));
	njAnim->SetPosition (Vector ( 8.0, 15.0, 0  ));
	nkAnim->SetPosition (Vector ( 10.0, 15.0, 0  ));

	anim.UpdateNodeDescription(11, "R1");
	anim.UpdateNodeDescription(12, "R2");
	anim.UpdateNodeDescription(13, "R3");
	anim.UpdateNodeDescription(14, "R4");
	anim.UpdateNodeDescription(15, "R5");
	anim.UpdateNodeDescription(16, "R6");

	anim.UpdateNodeDescription(0, "A");
	anim.UpdateNodeDescription(1, "B");
	anim.UpdateNodeDescription(2, "C");
	anim.UpdateNodeDescription(3, "D");
	anim.UpdateNodeDescription(4, "E");
	anim.UpdateNodeDescription(5, "F");
	anim.UpdateNodeDescription(6, "G");
	anim.UpdateNodeDescription(7, "H");
	anim.UpdateNodeDescription(8, "I");
	anim.UpdateNodeDescription(9, "J");
	anim.UpdateNodeDescription(10, "K");


	/*------------------------------------------------- Start simulation ----------------------------------*/
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Stop (Seconds (90.0));
	Simulator::Run ();
	Simulator::Destroy ();
	NS_LOG_INFO ("Done.");
}



