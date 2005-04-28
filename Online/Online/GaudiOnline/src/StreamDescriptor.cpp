//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//	Author     : M.Frank
//====================================================================
// $Id: StreamDescriptor.cpp,v 1.7 2005-04-28 11:53:19 bgaidioz Exp $

// Include files
#include "GaudiOnline/StreamDescriptor.h"
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <exception>
namespace SFC {
  #include "SFCClient/SFCClient.h"
}
#ifdef _WIN32
  #include <io.h>
#else
  #include <ctype.h>
  #include <unistd.h>
static const int _O_BINARY = 0;
static const int _O_RDONLY = O_RDONLY;
static const int _O_WRONLY = O_WRONLY;
static const int _O_CREAT  = O_CREAT;
#endif

namespace Networking {
#ifdef _WIN32
  #include "Winsock2.h"
  struct __init__ {
    __init__()  {
      static bool g_first = true;
      if ( g_first )  {
        g_first = false;
        static WSADATA g_WSAData;
        memset(&g_WSAData, 0, sizeof(WSADATA));
        if (WSAStartup ( MAKEWORD(1,1), &g_WSAData) != 0)    {
          std::runtime_error("GaudiOnline::StreamDescriptor> WSAStartup failed!");
        }
      }
    }
  };
  static __init__ g_init;
#else
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  int (*closesocket)(int) = ::close;
#endif
  static const int _SOCK_STREAM = SOCK_STREAM;
  static const int _IPPROTO_IP  = _IPPROTO_IP;
}
namespace FileIO {
  using ::close;
  using ::open;
  using ::read;
  using ::write;
}


namespace {
  typedef GaudiOnline::StreamDescriptor::Access Access;
  bool file_read(const Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = FileIO::read(con.ioDesc,p+tmp,len-tmp);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool file_read_len(const Access& con, int& len)  {
    return file_read(con, &len, sizeof(len));
  }
  bool file_write(const Access& con, const void* data, int len)  {
    const char* p = (const char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = FileIO::write(con.ioDesc,p+len-tmp,tmp);
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  /// Fast functions: set trigger decision
  bool file_set_decision(const Access& /* con */, int /* value */ )   {
    return true;
  }
  /// fast functions: send trigger decision
  bool file_send_decision(const Access& )  {
    return true;
  }
  bool ip_recv(const Access& con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = Networking::recv(con.ioDesc,p+tmp,len-tmp,0);
      if ( sc > 0 ) tmp += sc;
      else          return false;
    }
    return true;
  }
  bool ip_recv_len(const Access& con, int& len)  {
    return ip_recv(con, &len, sizeof(len));
  }
  bool ip_send(const Access& con, const void* data, int len)  {
    char* p = (char*)data;
    int tmp = len;
    while ( tmp>0 )  {
      int sc = Networking::send(con.ioDesc,p+len-tmp,tmp,0);
      if ( sc > 0 ) tmp -= sc;
      else          return false;
    }
    return true;
  }
  /// Fast functions: set trigger decision
  bool ip_set_decision(const Access& /* con */, int /* value */ )   {
    return true;
  }
  /// fast functions: send trigger decision
  bool ip_send_decision(const Access& )  {
    return true;
  }
  bool sfc_recv(const Access& /* con */, void* data, int len)  {
    SFC::sfcc_event_buffer rd_buff;
    rd_buff.buffer = (char*)data;
    rd_buff.buffer_len = rd_buff.max_len = len;
    return SFC::sfcc_read_event(&rd_buff) == 0;
  }
  bool sfc_recv_len(const Access& /* con */, int& len)  {
    return SFC::sfcc_read_length(&len) == 0;
  }
  bool sfc_send(const Access& /* con */, const void* /* data */, int /* len */)  {
    //SFC::sfcc_send_buffer snd_buff;
    //snd_buff.buffer = (char*)data;
    //snd_buff.buffer_len = len;
    return false; //return SFC::sfcc_send(&snd_buff) == 0;
  }
  /// Fast functions: set trigger decision
  bool sfc_set_decision(const Access& /* con */, int value )   {
    static int dec = value;
    SFC::sfcc_decision dec_buff;
    dec_buff.buffer_len = sizeof(int);
    dec_buff.buffer = (SFC::SFCC_C_CHAR*)&dec;
    return SFC::sfcc_set_decision(&dec_buff) == 0;
  }
  /// fast functions: send trigger decision
  bool sfc_send_decision(const Access& /* con */)  {
    return SFC::sfcc_push_decision() == 0;
  }
}

// Standard Constructor
GaudiOnline::StreamDescriptor::StreamDescriptor()
: m_allocated(0), m_length(0), m_data(0), m_decision(NONE), m_type(NONE), m_currentAccess(0)
{
}

// Standard Destructor
GaudiOnline::StreamDescriptor::~StreamDescriptor()
{
  allocate(0);
}

/// Allocate data block
char* GaudiOnline::StreamDescriptor::allocate(int len)  {
  m_length = 0;
  if ( len > 0 )  {
    if ( len < m_allocated )  {
      return m_data;
    }
    if ( m_data ) delete [] m_data;
    m_data = new char[m_allocated = len];
    return m_data;
  }
  if ( m_data ) delete [] m_data;
  m_allocated = 0;
  m_data = 0;
  return m_data;
}

void GaudiOnline::StreamDescriptor::getFileConnection(const std::string& con, std::string& file)  {
  size_t idx = con.find("://");
  file = "";
  if ( idx != std::string::npos )  {
    file = con.substr(idx+3);
  }
}

void GaudiOnline::StreamDescriptor::getInetConnection(const std::string& con, 
                                                      std::string& host,
                                                      Networking::in_addr* ip,
                                                      unsigned short& port)
{
  getFileConnection(con, host);
  size_t idx = host.find(":");
  ip->s_addr = port = 0;
  if ( idx != std::string::npos )  {
    std::string prt = host.substr(idx+1);
    ::sscanf(prt.c_str(),"%hd",&port);
#ifdef ntohs
    port = ntohs(port);
#else
    port = Networking::ntohs(port);
#endif
    host = host.substr(0,idx);
  }
  if ( ::isalpha(host[0]) )  {
    Networking::hostent* h = Networking::gethostbyname(host.c_str());
    if ( h )  {
      if ( h->h_addr_list[0] )  {
        ip->s_addr = *(unsigned long*)h->h_addr_list[0];
      }
    }
  }
  else {
    ip->s_addr = Networking::inet_addr(host.c_str());
  }
}

int GaudiOnline::StreamDescriptor::dataType(const std::string& val)  {
  if ( val == "DAQ" || val == "L2" || val == "L3" )
    return DAQ_BUFFER;
  else if ( val == "L1" )
    return L1_BUFFER;
  return NONE;
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::connect(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      StreamDescriptor::getFileConnection(specs, file);
      result.ioDesc     = FileIO::open(file.c_str(), _O_RDONLY|_O_BINARY );
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_read_len = file_read_len;
      result.m_set_decision  = file_set_decision;
      result.m_send_decision = file_send_decision;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      /*
        struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
        };
      */
      getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
      result.ioDesc = Networking::socket(AF_INET, Networking::_SOCK_STREAM, Networking::_IPPROTO_IP);
      if ( result.ioDesc > 0 )   {        
        sin.sin_family      = AF_INET;
        ::memset(sin.sin_zero,0,sizeof(sin.sin_zero));
        int ret = Networking::connect(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin));
        if ( ret == 0 )  {
          result.m_write    = ip_send;
          result.m_read     = ip_recv;
          result.m_read_len = ip_recv_len;
          result.m_set_decision  = ip_set_decision;
          result.m_send_decision = ip_send_decision;
          return result;
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
        break;
      }
      break;
    case 'S':          //  DATA='sfc://137.138.142.82:8000'
      //StreamDescriptor::getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
      if ( 0 == SFC::sfcc_register(file.c_str(), SFCC_L1) )  {
        result.ioDesc   = 1;
        result.m_write    = sfc_send;
        result.m_read     = sfc_recv;
        result.m_read_len = sfc_recv_len;
        result.m_set_decision  = sfc_set_decision;
        result.m_send_decision = sfc_send_decision;
        return result;
      }
      break;
    default:
      break;
  }
  return result;
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::bind(const std::string& specs)  {
  Access result;
  std::string file;
  Networking::sockaddr_in sin;
  result.type = ::toupper(specs[0]);
  switch(result.type) {
    case 'F':          //  DATA='file://C:/Data/myfile.dat'
      StreamDescriptor::getFileConnection(specs, file);
#ifdef _WIN32
      result.ioDesc     = FileIO::open(file.c_str(), _O_WRONLY|_O_BINARY|_O_CREAT );
#else
      result.ioDesc     = FileIO::open(file.c_str(), _O_WRONLY|_O_BINARY|_O_CREAT, S_IRWXU );
#endif
      result.m_write    = file_write;
      result.m_read     = file_read;
      result.m_read_len = file_read_len;
      result.m_set_decision  = file_set_decision;
      result.m_send_decision = file_send_decision;
      break;
    case 'I':          //  DATA='ip://137.138.142.82:8000'
      result.ioDesc = Networking::socket(AF_INET,Networking::_SOCK_STREAM,Networking::_IPPROTO_IP);
      if ( result.ioDesc > 0 )   {
        StreamDescriptor::getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
        sin.sin_family = AF_INET;
        if ( Networking::bind(result.ioDesc, (Networking::sockaddr*)&sin, sizeof(sin)) == 0) {
          if ( Networking::listen(result.ioDesc, SOMAXCONN) == 0 )  {
            result.m_write    = ip_send;
            result.m_read     = ip_recv;
            result.m_read_len = ip_recv_len;
            result.m_set_decision  = ip_set_decision;
            result.m_send_decision = ip_send_decision;
            return result;
          }
        }
        Networking::closesocket(result.ioDesc);
        result.ioDesc = -1;
      }
      break;
    case 'S':          //  DATA='sfc://137.138.142.82:8000'
      StreamDescriptor::getInetConnection(specs, file, &sin.sin_addr, sin.sin_port);
      if ( 0 == SFC::sfcc_register(file.c_str(), sin.sin_port) )  {
        result.ioDesc     = 1;
        result.m_write    = sfc_send;
        result.m_read     = sfc_recv;
        result.m_read_len = sfc_recv_len;
        result.m_set_decision  = sfc_set_decision;
        result.m_send_decision = sfc_send_decision;
        return result;
      }
      break;
    default:
      break;
  }
  return result;
}

int GaudiOnline::StreamDescriptor::close(Access& specs) {
  int dsc = specs.ioDesc; 
  specs.ioDesc = -1; 
  switch(specs.type)  {
    case 'F':
      return dsc > 0 ? FileIO::close(dsc) : 0;
    case 'I':
      return dsc > 0 ? Networking::closesocket(dsc) : 0;
    case 'S':
      return SFC::sfcc_unregister();
    default:
      return false;
  }
}

GaudiOnline::StreamDescriptor::Access 
GaudiOnline::StreamDescriptor::accept(const Access& specs)  {
  Access result = specs;
  switch(specs.type)  {
    case 'F':
      result.ioDesc = specs.ioDesc;
      break;
    case 'I':
      if ( specs.ioDesc > 0 )  {
#ifdef _WIN32
        int len;
#else
        size_t len;
#endif
        Networking::sockaddr sin;
        ::memset(&sin,0,sizeof(sin));
        result.ioDesc = Networking::accept(specs.ioDesc, &sin, &len);
      }
      break;
    case 'S':
      result.ioDesc = 1;
      break;
    default:
      result.ioDesc = -1;
      break;
  }
  return result;
}

/// Send decision
int GaudiOnline::StreamDescriptor::sendDecision()  const {
  if ( m_currentAccess )  {
    return sendDecision(*m_currentAccess) ? 1 : 0;
  }
  return 0;
}

/// Send decision
int GaudiOnline::StreamDescriptor::setDecision(int val)  const {
  if ( m_currentAccess )  {
    return setDecision(*m_currentAccess,val) ? 1 : 0;
  }
  return 0;
}
