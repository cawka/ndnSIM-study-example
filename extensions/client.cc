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
  OnData (Ptr< const ndn::Data > data)
  {
    ndn::App::OnData (data); // for proper logging

    NS_LOG_DEBUG ("<< ndnSIM data " << data->GetName ());
    
    Simulator::Remove (m_interestTimeoutEvent); // cancel interest timeout

    // print out payload ("content") to the standard output
    data->GetPayload ()->CopyData (&std::cout, data->GetPayload ()->GetSize ());

    int seqno = data->GetName ().get (-1).toSeqNum ();
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
    Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
    interest->SetName (ndn::Name (m_baseInterestName).appendSeqNum (seqno));
    interest->SetInterestLifetime (Seconds (2.0));
    interest->SetNonce (m_rand.GetValue ());

    // send packet out to the NDN stack
    Simulator::ScheduleNow (&ndn::Face::ReceiveInterest, m_face, interest);
    m_transmittedInterests (interest, this, m_face);

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
