#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/ndnSIM-module.h>

using namespace std;
using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  Ptr<Node> node1 = CreateObject<Node> ();
  Ptr<Node> node2 = CreateObject<Node> ();
  Ptr<Node> node3 = CreateObject<Node> ();

  // or
  // NodeContainer nodes;
  // nodes.Create (3);

  // connect nodes with point-to-point links
  PointToPointHelper p2pHelper;
  p2pHelper.Install (node1, node2);
  p2pHelper.Install (node2, node3);

  // configure and install NDN stack on nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "100");
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
  ndnHelper.InstallAll ();
  // or
  // ndnHelper.Install (nodes);
  // or
  // ndnHelper.Install (node1);
  // ndnHelper.Install (node2);
  // ndnHelper.Install (node3);

  // configure FIBs
  ndnHelper.AddRoute (node1, "/", node2, 1);
  ndnHelper.AddRoute (node2, "/", node3, 1);

  // create and install applications
  ndn::AppHelper clientAppHelper ("Client");
  ApplicationContainer clientApp = clientAppHelper.Install (node1);
  clientApp.Start (Seconds (1.0));
  clientApp.Stop (Seconds (10.0));

  ndn::AppHelper serverAppHelper ("Server");
  ApplicationContainer serverApp = serverAppHelper.Install (node3);
  // serverApp will start at simulation time 0 and will be running throughout the simulation

  // Define duration of the simulation
  Simulator::Stop (Seconds (20.0));

  // Run the simulation
  Simulator::Run ();

  // Cleanup after the simulation
  Simulator::Destroy ();

  return 0;
}
