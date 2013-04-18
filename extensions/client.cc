#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace ns3;

// for NS-3 debug logging
NS_LOG_COMPONENT_DEFINE ("Client");

class Client : public ndn::App
{
public:
  static TypeId
  GetTypeId ()
  {
    static TypeId tid = TypeId ("Client")
      .SetParent<ndn::App> ()
      .AddConstructor<Client> ();
    return tid;
  }
  
  Client ()
    : m_baseInterestName ("/my-local-prefix/simple-fetch/file/foobar")
  {
  }
  
  void
  OnContentObject (const Ptr< const ndn::ContentObject > &contentObject, Ptr< Packet > payload)
  {
    ndn::App::OnContentObject (contentObject, payload); // for proper logging

    NS_LOG_DEBUG ("<< ndnSIM data " << contentObject->GetName ());
    
    Simulator::Remove (m_interestTimeoutEvent); // cancel interest timeout

    // print out payload ("content") to the standard output
    payload->CopyData (&std::cout, payload->GetSize ());

    int seqno = boost::lexical_cast<int> (contentObject->GetName ().GetLastComponent ());
    if (seqno >= 10)
      {
        return;
      }

    SendInterest (seqno + 1);
  }

protected:
  void
  StartApplication ()
  {
    ndn::App::StartApplication (); // creating internal NDN "handler"

    SendInterest (0);
  }

private:
  void
  SendInterest (int seqno)
  {
    ndn::Interest interest;
    interest.SetName (ndn::Name (m_baseInterestName)(seqno));
    // same as: interest.SetName (ndn::Name (m_baseInterestName).Add (seqno));
    interest.SetInterestLifetime (Seconds (2.0));
    interest.SetNonce (m_rand.GetValue ());
    
    // some standard mechanics to create packet with Interest "header"
    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (interest);
    m_transmittedInterests (&interest, this, m_face);

    // send packet out to the NDN stack
    m_protocolHandler (packet);

    m_interestTimeoutEvent = Simulator::Schedule (Seconds (2.01), &Client::OnTimeout, this, seqno);
  }

  void
  OnTimeout (int seqno)
  {
    NS_LOG_DEBUG ("Interest for segment " << seqno << " timed out. Re-expressing");
    SendInterest (seqno);
  }
  
private:
  UniformVariable m_rand;
  ndn::Name m_baseInterestName;

  EventId m_interestTimeoutEvent;
};

NS_OBJECT_ENSURE_REGISTERED (Client);
