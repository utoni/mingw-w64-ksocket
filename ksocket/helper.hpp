#ifndef HELPER_HPP
#define HELPER_HPP 1

#include <protobuf-c/protobuf-c.h>
#include <stdint.h>
#include <stdlib.h>

#include <EASTL/algorithm.h>
#include <EASTL/array.h>
#include <EASTL/initializer_list.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#define SEND_ALL(sock, socket_buffer, retval)                                  \
  if (socket_buffer.GetRemainingSendSize() > 0) {                              \
    do {                                                                       \
      retval = send(sock, socket_buffer.GetStartS(),                           \
                    socket_buffer.GetRemainingSendSize(), 0);                  \
      if (retval == SOCKET_ERROR || retval == 0)                               \
        break;                                                                 \
      if (!socket_buffer.Consume(iResult))                                     \
        break;                                                                 \
    } while (!socket_buffer.AllConsumed());                                    \
    socket_buffer.Sweep();                                                     \
  }
#define RECV_PDU_BEGIN(sock, socket_buffer, retval, pdu_type, pdu_len)         \
  do {                                                                         \
    if (socket_buffer.GetRemainingRecvSize() == 0)                             \
      break;                                                                   \
    uint16_t pdu_type;                                                         \
    uint32_t pdu_len;                                                          \
    do {                                                                       \
      retval = recv(sock, socket_buffer.GetStartS(),                           \
                    socket_buffer.GetRemainingRecvSize(), 0);                  \
      if (retval == SOCKET_ERROR || retval == 0)                               \
        break;                                                                 \
    } while (!socket_buffer.GetPdu(retval, pdu_type, pdu_len));
#define RECV_PDU_END(socket_buffer, pdu_len)                                   \
  socket_buffer.Consume(pdu_len);                                              \
  socket_buffer.Sweep();                                                       \
  }                                                                            \
  while (0)

class BaseSerializer {
public:
  virtual uint16_t GetPduType(void) = 0;
  virtual size_t GetSerializedSize(void) = 0;
  virtual size_t Serialize(uint8_t *buf) = 0;
};

class BaseDeserializer {
public:
  virtual bool Deserialize(size_t pdu_len, uint8_t *buf) = 0;
  virtual void DeserializeFree(void) = 0;
};

template <size_t SIZE> class SocketBuffer {
public:
  SocketBuffer(void) {}
  ~SocketBuffer(void) {}
  size_t GetRemainingSendSize(void) { return GetUsed(); }
  size_t GetRemainingRecvSize(void) { return GetSize() - GetUsed(); }
  uint8_t *GetStart(void) { return buffer + consumed; }
  char *GetStartS(void) { return (char *)buffer + consumed; }
  bool SizeCheck(size_t required_size) { return used + required_size < SIZE; };
  template <typename T> bool GetPrimitve(size_t offset, T &out) {
    if (offset + sizeof(out) > used)
      return false;
    out = *(T *)(GetStart() + offset);
    return true;
  }
  template <typename T> bool AddPrimitve(T value) {
    if (!SizeCheck(sizeof(value)))
      return false;
    *(T *)(GetEnd()) = value;
    used += sizeof(value);
    return true;
  }
  bool GetPdu(size_t received_size, uint16_t &pdu_type, uint32_t &pdu_len) {
    if (received_size > SIZE - used)
      return false; // You did something wrong!
    used += received_size;
    if (used < sizeof(pdu_type) + sizeof(pdu_len))
      return false;
    if (GetPrimitve<uint32_t>(0, pdu_len) == false)
      return false;
    if (GetPrimitve<uint16_t>(4, pdu_type) == false)
      return false;

    pdu_len = ntohl(pdu_len);
    pdu_type = ntohs(pdu_type);

    if (used < sizeof(pdu_type) + sizeof(pdu_len) + pdu_len)
      return false;

    consumed += sizeof(pdu_type) + sizeof(pdu_len);

    return true;
  }
  bool AddPdu(BaseSerializer &bs) {
    uint16_t pdu_type = bs.GetPduType();
    uint32_t pdu_len = bs.GetSerializedSize();

    if (!SizeCheck(sizeof(pdu_type) + sizeof(pdu_len) + pdu_len))
      return false;

    if (!AddPrimitve<uint32_t>(htonl(pdu_len)))
      return false;
    if (!AddPrimitve<uint16_t>(htons(pdu_type)))
      return false;
    if (bs.Serialize(GetEnd()) != pdu_len)
      return false;
    used += pdu_len;

    return true;
  }
  bool AllConsumed(void) { return used == consumed; }
  bool Consume(size_t consuming_size) {
    if (consuming_size + consumed > used)
      return false;
    consumed += consuming_size;
    return true;
  }
  void Sweep() {
    if (consumed == 0)
      return;
    if (used != consumed)
      memmove(buffer, buffer + consumed, used - consumed);
    used -= consumed;
    consumed = 0;
  }

private:
  uint8_t buffer[SIZE];
  size_t used = 0;
  size_t consumed = 0;

  size_t GetSize(void) { return SIZE; }
  size_t GetUsed(void) { return used - consumed; }
  uint8_t *GetEnd() { return buffer + used; }
};

class ProtobufCBinaryDataClass : public ProtobufCBinaryData {
public:
  ProtobufCBinaryDataClass(std::initializer_list<uint8_t> bytes) {
    len = bytes.size();
    data = new uint8_t[len];
    eastl::copy(bytes.begin(), bytes.end(), data);
  }
  ~ProtobufCBinaryDataClass(void) { delete data; }
};

#endif
