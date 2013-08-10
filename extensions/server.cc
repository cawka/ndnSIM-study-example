#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>

using namespace std;
using namespace ns3;

// for NS-3 debug logging
NS_LOG_COMPONENT_DEFINE ("Server");

class Server : public ndn::App
{
public:
  static TypeId
  GetTypeId ()
  {
    static TypeId tid = TypeId ("Server")
      .SetParent<ndn::App> ()
      .AddConstructor<Server> ();
    return tid;
  }
  
  void
  OnInterest (Ptr< const ndn::Interest > interest)
  {
    ndn::App::OnInterest (interest); // for proper logging
    
    NS_LOG_DEBUG ("<< ndnSIM interest " << interest->GetName ());

    static int COUNTER = 0;
    std::ostringstream os;
    os << "ndnSIM LINE #" << (COUNTER++) << endl;
    
    // create packet with meaningful payload ("content")
    Ptr<ndn::Data> data = Create<ndn::Data> (Create<Packet> (reinterpret_cast<const uint8_t *> (os.str ().c_str ()),
                                                             os.str ().size ()+1));
    data->SetName (interest->GetName ());
    data->SetFreshness (Seconds (5.0));

    // send packet out to the NDN stack to "satisfy" the interest
    Simulator::ScheduleNow (&ndn::Face::ReceiveData, m_face, data);

    m_transmittedDatas (data, this, m_face);
  }

protected:
  void
  StartApplication ()
  {
    ndn::App::StartApplication (); // creating internal NDN "handler"
  
    // "set interest filter" = set up a FIB entry
    GetNode ()
      ->GetObject<ndn::Fib> ()
      ->Add (ndn::Name ("/my-local-prefix/simple-fetch/file"), m_face, 0);
  }
};

NS_OBJECT_ENSURE_REGISTERED (Server);
