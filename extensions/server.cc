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
  OnInterest (const Ptr< const ndn::Interest > &interest, Ptr< Packet > origPacket)
  {
    ndn::App::OnInterest (interest, origPacket); // for proper logging
    
    NS_LOG_DEBUG ("<< ndnSIM interest " << interest->GetName ());

    static int COUNTER = 0;
    std::ostringstream os;
    os << "ndnSIM LINE #" << (COUNTER++) << endl;
    
    ndn::ContentObject data;
    data.SetName (interest->GetName ());

    // create packet with meaningful payload ("content")
    Ptr<Packet> packet = Create<Packet> (reinterpret_cast<const uint8_t *> (os.str ().c_str ()),
                                         os.str ().size ()+1);
    // Ptr<Packet> packet = Create<Packet> (1000); <-- creates a packet with 1000-byte virtual payload

    packet->AddHeader (data);
    m_transmittedContentObjects (&data, packet, this, m_face);

    // send packet out to the NDN stack to "satisfy" the interest
    m_protocolHandler (packet);
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
