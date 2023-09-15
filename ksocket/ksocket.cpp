#include "ksocket.hpp"

#include <EASTL/type_traits.h>
#include <eastl_compat.hpp>

#ifdef BUILD_USERMODE
// clang-format off
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
// clang-format on
#else
#include <ksocket/berkeley.h>
#include <ksocket/ksocket.h>
#include <ksocket/wsk.h>
#endif
#include <ksocket/utils.h>

struct KSocketImplCommon {
  ADDRESS_FAMILY domain;
  int type;
  int proto;
};

#ifdef BUILD_USERMODE
struct KSocketImpl {
  ~KSocketImpl() {
    if (s != INVALID_SOCKET) {
      closesocket(s);
      s = INVALID_SOCKET;
    }
  }

  SOCKET s = INVALID_SOCKET;
  KSocketImplCommon c;
};
#else
struct KSocketImpl {
  ~KSocketImpl() {
    if (s >= 0) {
      closesocket(s);
      s = -1;
    }
  }

  int s = -1;
  KSocketImplCommon c;
};
#endif

eastl::string KSocketAddress::to_string(bool with_port) const {
  if (addr_used != 4) {
    return "";
  }

  return ::to_string(addr.u8[0]) + "." + ::to_string(addr.u8[1]) + "." +
         ::to_string(addr.u8[2]) + "." + ::to_string(addr.u8[3]) + ":" +
         ::to_string(with_port);
}

void KSocketBuffer::insert_u8(KBuffer::iterator it, uint8_t value) {
  buffer.insert(it, value);
}

void KSocketBuffer::insert_u16(KBuffer::iterator it, uint16_t value) {
  uint16_t net_value;
  uint8_t insert_value[2];

  net_value = htons(value);
  insert_value[0] = (net_value & 0x00FF) >> 0;
  insert_value[1] = (net_value & 0xFF00) >> 8;
  buffer.insert(it, insert_value, insert_value + eastl::size(insert_value));
}

void KSocketBuffer::insert_u32(KBuffer::iterator it, uint32_t value) {
  uint32_t net_value;
  uint8_t insert_value[4];

  net_value = htonl(value);
  insert_value[0] = (net_value & 0x000000FF) >> 0;
  insert_value[1] = (net_value & 0x0000FF00) >> 8;
  insert_value[2] = (net_value & 0x00FF0000) >> 16;
  insert_value[3] = (net_value & 0xFF000000) >> 24;
  buffer.insert(it, insert_value, insert_value + eastl::size(insert_value));
}

void KSocketBuffer::insert_u64(KBuffer::iterator it, uint64_t value) {
  uint64_t net_value;
  uint8_t insert_value[8];

  net_value = htonll(value);
  insert_value[0] = (net_value & 0x00000000000000FF) >> 0;
  insert_value[1] = (net_value & 0x000000000000FF00) >> 8;
  insert_value[2] = (net_value & 0x0000000000FF0000) >> 16;
  insert_value[3] = (net_value & 0x00000000FF000000) >> 24;
  insert_value[4] = (net_value & 0x000000FF00000000) >> 32;
  insert_value[5] = (net_value & 0x0000FF0000000000) >> 40;
  insert_value[6] = (net_value & 0x00FF000000000000) >> 48;
  insert_value[7] = (net_value & 0xFF00000000000000) >> 56;
  buffer.insert(it, insert_value, insert_value + eastl::size(insert_value));
}

void KSocketBuffer::insert_bytebuffer(KBuffer::iterator it,
                                      const uint8_t bytebuffer[], size_t size) {
  buffer.insert(it, bytebuffer, bytebuffer + size);
}

void KSocketBuffer::consume(size_t amount_bytes) {
  if (amount_bytes == 0 || amount_bytes > buffer.size())
    amount_bytes = buffer.size();

  buffer.erase(buffer.begin(), buffer.begin() + amount_bytes);
}

eastl::string KSocketBuffer::toHex(eastl::string delim) {
  eastl::string str;
  char const *const hex = "0123456789ABCDEF";
  char pout[3] = {};

  for (const auto &input_byte : buffer) {
    pout[0] = hex[(input_byte >> 4) & 0xF];
    pout[1] = hex[input_byte & 0xF];
    str += pout;
    str += delim;
  }

  if (str.length() >= delim.length() && delim.length() > 0)
    str.erase(str.length() - delim.length(), delim.length());

  return str;
}

KSocket::~KSocket() {
  if (m_socket != nullptr)
    delete m_socket;
}

bool KSocket::setup(KSocketType sock_type, int proto) {
  int domain, type;

  m_lastError = KSE_SUCCESS;
  if (m_socket != nullptr)
    delete m_socket;
  m_socket = new KSocketImpl();
  if (m_socket == nullptr) {
    m_lastError = KSE_SETUP_IMPL_NULL;
    return false;
  }

  if (KSocket::socketTypeToTuple(sock_type, domain, type) != true) {
    m_lastError = KSE_SETUP_INVALID_SOCKET_TYPE;
    return false;
  }
  if (sock_type == KSocketType::KST_STREAM_CLIENT_IP6 ||
      sock_type == KSocketType::KST_STREAM_SERVER_IP6 ||
      sock_type == KSocketType::KST_DATAGRAM_IP6) {
    m_lastError = KSE_SETUP_UNSUPPORTED_SOCKET_TYPE;
    return false; // IPv6 is not supported for now
  }
  m_socketType = sock_type;

#ifdef BUILD_USERMODE
  m_socket->s = ::socket(domain, type, proto);
#else
  // DbgPrint("KSocketType: %d, domain: %d, type: %d, proto: %d", sock_type,
  // domain, type, proto);
  switch (sock_type) {
  case KSocketType::KST_INVALID:
    m_lastError = KSE_SETUP_INVALID_SOCKET_TYPE;
    return false;
  case KSocketType::KST_STREAM_CLIENT_IP4:
  case KSocketType::KST_STREAM_CLIENT_IP6:
    m_socket->s = ::socket_connection(domain, type, proto);
    break;
  case KSocketType::KST_STREAM_SERVER_IP4:
  case KSocketType::KST_STREAM_SERVER_IP6:
    m_socket->s = ::socket_listen(domain, type, proto);
    break;
  case KSocketType::KST_DATAGRAM_IP4:
  case KSocketType::KST_DATAGRAM_IP6:
    m_socket->s = ::socket_datagram(domain, type, proto);
    break;
  }
#endif
  m_socket->c.domain = static_cast<ADDRESS_FAMILY>(domain);
  m_socket->c.type = type;
  m_socket->c.proto = proto;
#ifdef BUILD_USERMODE
  return m_socket->s != INVALID_SOCKET;
#else
  return m_socket->s >= 0;
#endif
}

bool KSocket::connect(eastl::string host, eastl::string port) {
  struct addrinfo hints = {};
  struct addrinfo *results;

  m_lastError = KSE_SUCCESS;

  if (!sanityCheck())
    return false;

  if (m_socketType != KSocketType::KST_STREAM_CLIENT_IP4 &&
      m_socketType != KSocketType::KST_STREAM_CLIENT_IP6)
    return false;

  hints.ai_flags |= AI_CANONNAME;
  hints.ai_family = m_socket->c.domain;
  hints.ai_socktype = m_socket->c.type;
  m_lastError = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &results);
  if (m_lastError != KSE_SUCCESS)
    return false;

  m_lastError =
      ::connect(m_socket->s, results->ai_addr, (int)results->ai_addrlen);
  freeaddrinfo(results);
  return m_lastError == KSE_SUCCESS;
}

bool KSocket::bind(uint16_t port) {
  struct sockaddr_in addr;

  if (!sanityCheck())
    return false;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  m_lastError = ::bind(m_socket->s, (struct sockaddr *)&addr, sizeof(addr));

  return m_lastError == KSE_SUCCESS;
}

bool KSocket::listen(int backlog) {
  m_lastError = KSE_SUCCESS;

  if (!sanityCheck())
    return false;

  if (m_socketType != KSocketType::KST_STREAM_SERVER_IP4 &&
      m_socketType != KSocketType::KST_STREAM_SERVER_IP6)
    return false;

  m_lastError = ::listen(m_socket->s, backlog);

  return m_lastError == KSE_SUCCESS;
}

bool KSocket::accept(KAcceptThreadCallback thread_callback) {
  KAcceptedSocket ka;
  struct sockaddr addr;
  socklen_t addrlen = sizeof(addr);

  if (!sanityCheck())
    return false;

  if (m_socket->c.domain != AF_INET) {
    m_lastError = KSE_SETUP_UNSUPPORTED_SOCKET_TYPE;
    return false;
  }

  addr.sa_family = m_socket->c.domain;
  ka.m_socket = new KSocketImpl();
  if (ka.m_socket == nullptr) {
    m_lastError = KSE_SETUP_IMPL_NULL;
    return false;
  }
  ka.m_socketType = m_socketType;
  ka.m_socket->c = m_socket->c;
  ka.m_socket->s = ::accept(m_socket->s, &addr, &addrlen);

  if (m_socket->c.domain == AF_INET) {
    struct sockaddr_in *addr_in = reinterpret_cast<struct sockaddr_in *>(&addr);

    ka.m_remote.addr_used = 4;
    ka.m_remote.addr.u32[0] = addr_in->sin_addr.s_addr;
    ka.m_remote.port = addr_in->sin_port;
  } else {
    m_lastError = KSE_SETUP_UNSUPPORTED_SOCKET_TYPE;
    return false;
  }

#ifdef BUILD_USERMODE
  if (ka.m_socket->s == INVALID_SOCKET)
#else
  if (ka.m_socket->s < 0)
#endif
  {
    m_lastError = KSE_ACCEPT_FAILED;
    return false;
  }

  return thread_callback(ka);
}

bool KSocket::close() {
  int rv;

  if (!sanityCheck())
    return false;

  rv = closesocket(m_socket->s);

  if (rv == 0) {
    m_socket->s = -1;
    return true;
  } else {
    return false;
  }
}

bool KSocket::send() {
  m_lastError = KSE_SUCCESS;

  if (!sanityCheck())
    return false;

  if (m_sendBuffer.size() == 0)
    return false;

  m_lastError =
      ::send(m_socket->s, reinterpret_cast<const char *>(m_sendBuffer.data()),
             m_sendBuffer.size(), 0);
  if (m_lastError > 0) {
    m_sendBuffer.buffer.erase(m_sendBuffer.begin(),
                              m_sendBuffer.begin() + m_lastError);
    return true;
  }

  return false;
}

bool KSocket::recv(size_t max_recv_size) {
  const size_t current_size = m_recvBuffer.size();

  if (!sanityCheck())
    return false;

  m_recvBuffer.buffer.resize(current_size + max_recv_size);
  m_lastError = ::recv(
      m_socket->s, reinterpret_cast<char *>(m_recvBuffer.data() + current_size),
      max_recv_size, 0);
  if (m_lastError > 0) {
    m_recvBuffer.buffer.resize(current_size + m_lastError);
    return true;
  }

  return false;
}

bool KSocket::sanityCheck() {
  if (m_socket != nullptr &&
#ifdef BUILD_USERMODE
      m_socket->s != INVALID_SOCKET
#else
      m_socket->s >= 0
#endif
  ) {
    return true;
  }

  m_lastError = KSE_INVALID_SOCKET;
  return false;
}

bool KSocket::socketTypeToTuple(KSocketType sock_type, int &domain, int &type) {
  switch (sock_type) {
  case KSocketType::KST_INVALID:
    break;
  case KSocketType::KST_STREAM_CLIENT_IP4:
  case KSocketType::KST_STREAM_SERVER_IP4:
    domain = AF_INET;
    type = SOCK_STREAM;
    return true;
  case KSocketType::KST_STREAM_CLIENT_IP6:
  case KSocketType::KST_STREAM_SERVER_IP6:
    domain = AF_INET6;
    type = SOCK_STREAM;
    return true;
  case KSocketType::KST_DATAGRAM_IP4:
  case KSocketType::KST_DATAGRAM_IP6:
    domain = AF_INET;
    type = SOCK_DGRAM;
    return true;
  }

  return false;
}
