#ifndef KSOCKET_HPP
#define KSOCKET_HPP 1

#include <EASTL/functional.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <cstdint>

using KBuffer = eastl::vector<uint8_t>;

struct KSocketImpl;

enum {
  KSE_SUCCESS = 0,
  KSE_SETUP_IMPL_NULL = 1,
  KSE_SETUP_INVALID_SOCKET_TYPE = 2,
  KSE_SETUP_UNSUPPORTED_SOCKET_TYPE = 3,
  KSE_ACCEPT_FAILED = 4,
};

enum class KSocketType {
  KST_INVALID = 0,
  KST_STREAM_CLIENT_IP4,
  KST_STREAM_SERVER_IP4,
  KST_STREAM_CLIENT_IP6,
  KST_STREAM_SERVER_IP6,
  KST_DATAGRAM_IP4,
  KST_DATAGRAM_IP6
};

struct KSocketAddress {
  eastl::string to_string(bool with_port = true) const;
  uint8_t addr_used = 0;
  union {
    uint8_t u8[16];
    uint16_t u16[8];
    uint32_t u32[4];
    uint64_t u64[2];
  } addr;
  uint16_t port = 0;
};

class KAcceptedSocket;
using KAcceptThreadCallback = eastl::function<bool(KAcceptedSocket &accepted)>;

struct KSocketBuffer {
  void insert_i8(KBuffer::iterator it, int8_t value) {
    insert_u8(it, static_cast<uint8_t>(value));
  }
  void insert_i16(KBuffer::iterator it, int16_t value) {
    insert_u16(it, static_cast<uint16_t>(value));
  }
  void insert_i32(KBuffer::iterator it, int32_t value) {
    insert_u32(it, static_cast<uint32_t>(value));
  }
  void insert_i64(KBuffer::iterator it, int64_t value) {
    insert_u64(it, static_cast<uint64_t>(value));
  }

  void insert_u8(KBuffer::iterator it, uint8_t value);
  void insert_u16(KBuffer::iterator it, uint16_t value);
  void insert_u32(KBuffer::iterator it, uint32_t value);
  void insert_u64(KBuffer::iterator it, uint64_t value);

  void insert_string(KBuffer::iterator it, const eastl::string &value) {
    insert_bytebuffer(it, reinterpret_cast<const uint8_t *>(value.c_str()),
                      value.length());
  }
  void insert_string(KBuffer::iterator it, const char buffer[], size_t size) {
    insert_bytebuffer(it, reinterpret_cast<const uint8_t *>(buffer), size);
  }
  void insert_bytebuffer(KBuffer::iterator it, const void *bytebuffer,
                         size_t size) {
    insert_bytebuffer(it, reinterpret_cast<const uint8_t *>(bytebuffer), size);
  }
  void insert_bytebuffer(KBuffer::iterator it, const uint8_t bytebuffer[],
                         size_t size);

  void insert(KBuffer::iterator it, int8_t value) {
    insert_u8(it, static_cast<uint8_t>(value));
  }
  void insert(KBuffer::iterator it, int16_t value) {
    insert_u16(it, static_cast<uint16_t>(value));
  }
  void insert(KBuffer::iterator it, int32_t value) {
    insert_u32(it, static_cast<uint32_t>(value));
  }
  void insert(KBuffer::iterator it, int64_t value) {
    insert_u64(it, static_cast<uint64_t>(value));
  }

  void insert(KBuffer::iterator it, uint8_t value) { insert_u8(it, value); }
  void insert(KBuffer::iterator it, uint16_t value) { insert_u16(it, value); }
  void insert(KBuffer::iterator it, uint32_t value) { insert_u32(it, value); }
  void insert(KBuffer::iterator it, uint64_t value) { insert_u64(it, value); }

  void insert(KBuffer::iterator it, const eastl::string value) {
    insert_string(it, eastl::move(value));
  }
  void insert(KBuffer::iterator it, const char buffer[], size_t size) {
    insert_string(it, buffer, size);
  }
  void insert(KBuffer::iterator it, const char *value) {
    insert_string(it, eastl::move(eastl::string(value)));
  }
  void insert(KBuffer::iterator it, const uint8_t bytebuffer[], size_t size) {
    insert_bytebuffer(it, bytebuffer, size);
  }

  void consume(size_t amount_bytes = 0);

  eastl::string to_string() {
    return eastl::string(reinterpret_cast<const char *>(data()), size());
  }
  uint8_t *data() { return buffer.data(); }
  size_t size() { return buffer.size(); }
  KBuffer::iterator begin() { return buffer.begin(); }
  KBuffer::iterator end() { return buffer.end(); }

  eastl::string toHex(eastl::string delim = ":");

  KBuffer buffer;
};

class KSocket {
protected:
  KSocket() : m_sendBuffer(), m_recvBuffer() {}
  KSocket(const KSocket &) = delete;
  ~KSocket();
  bool setup(KSocketType sock_type, int proto = 0);

  bool connect(eastl::string host, eastl::string port);
  bool bind(uint16_t port);
  bool listen(int backlog);
  bool accept(KAcceptThreadCallback thread_callback);
  bool close();

  bool send();
  bool recv(size_t max_recv_size);

public:
  int getLastError() const { return m_lastError; }

  KSocketBuffer &getSendBuffer() { return m_sendBuffer; }
  KSocketBuffer &getRecvBuffer() { return m_recvBuffer; }

  static bool socketTypeToTuple(KSocketType sock_type, int &domain, int &type);

private:
  KSocketBuffer m_sendBuffer;
  KSocketBuffer m_recvBuffer;
  KSocketType m_socketType = KSocketType::KST_INVALID;
  KSocketImpl *m_socket = nullptr;
  int m_lastError = KSE_SUCCESS;
};

class KAcceptedSocket : public KSocket {
  friend class KSocket;

public:
  KAcceptedSocket() : KSocket() {}
  ~KAcceptedSocket() {}
  bool setup(KSocketType sock_type, int proto = 0) = delete;

  bool connect(eastl::string host, eastl::string port) = delete;
  bool bind(uint16_t port) = delete;
  bool listen(int backlog) = delete;
  bool accept(KAcceptThreadCallback thread_callback) = delete;

  bool send() { return KSocket::send(); }
  bool recv(size_t max_recv_size = 65535) {
    return KSocket::recv(max_recv_size);
  }

  int getLastError() { return KSocket::getLastError(); }

  const KSocketAddress &getRemote() { return m_remote; }

private:
  KSocketAddress m_remote;
};

class KStreamClientIp4 : public KSocket {
public:
  KStreamClientIp4() : KSocket() {}
  ~KStreamClientIp4() {}

  bool setup() {
    return KSocket::setup(KSocketType::KST_STREAM_CLIENT_IP4,
                          6 /* IPPROTO_TCP */);
  }

  bool connect(eastl::string host, eastl::string port) {
    return KSocket::connect(host, port);
  }
  bool bind(uint16_t) = delete;
  bool listen(int) = delete;
  bool accept(KAcceptThreadCallback) = delete;

  bool send() { return KSocket::send(); }
  bool recv(size_t max_recv_size = 65535) {
    return KSocket::recv(max_recv_size);
  }

  int getLastError() { return KSocket::getLastError(); }
};

class KStreamServerIp4 : public KSocket {
public:
  KStreamServerIp4() : KSocket() {}
  ~KStreamServerIp4() {}

  bool setup() {
    return KSocket::setup(KSocketType::KST_STREAM_SERVER_IP4,
                          6 /* IPPROTO_TCP */);
  }

  bool connect(eastl::string host, eastl::string port) = delete;
  bool bind(uint16_t port) { return KSocket::bind(port); }
  bool listen(int backlog = 16) { return KSocket::listen(backlog); }
  bool accept(KAcceptThreadCallback thread_callback) {
    return KSocket::accept(thread_callback);
  }

  bool send() { return KSocket::send(); }
  bool recv(size_t max_recv_size = 65535) {
    return KSocket::recv(max_recv_size);
  }

  int getLastError() { return KSocket::getLastError(); }
};

#endif
