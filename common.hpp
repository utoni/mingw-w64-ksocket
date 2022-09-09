#include "protobuf-c/example.pb-c.h"

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

/* Could be also be done via protobuf, but I decided for speed/memory
 * efficiency. */
enum PduTypes {
  PDU_SOMETHING_WITH_UINTS = 1,
  PDU_SOMETHING_MORE = 2,
  PDU_EVEN_MORE = 3
};

class SomethingWithUINTsSerializer : virtual public BaseSerializer {
public:
  SomethingWithUINTsSerializer(void) {}
  ~SomethingWithUINTsSerializer(void) {}
  void SetId(uint32_t id) {
    swu.has_id = TRUE;
    swu.id = id;
  }
  void SetIpAddress(uint32_t ip_address) {
    swu.has_ip_address = TRUE;
    swu.ip_address = ip_address;
  }
  void SetPortNum(uint32_t port_num) {
    swu.has_port_num = TRUE;
    swu.port_num = port_num;
  }
  uint16_t GetPduType(void) override { return PDU_SOMETHING_WITH_UINTS; }
  size_t GetSerializedSize(void) override {
    return something_with_uints__get_packed_size(&swu);
  }
  size_t Serialize(uint8_t *buf) override {
    return something_with_uints__pack(&swu, buf);
  }

  SomethingWithUINTs swu = SOMETHING_WITH_UINTS__INIT;
};

class SomethingWithUINTsDeserializer : virtual public BaseDeserializer {
public:
  SomethingWithUINTsDeserializer(void) {}
  ~SomethingWithUINTsDeserializer(void) { DeserializeFree(); }
  bool Deserialize(size_t pdu_len, uint8_t *buf) override {
    swu = something_with_uints__unpack(NULL, pdu_len, buf);
    return swu != NULL;
  }
  void DeserializeFree(void) override {
    if (swu != NULL)
      something_with_uints__free_unpacked(swu, NULL);
  }

  SomethingWithUINTs *swu = NULL;
};

class SomethingMoreSerializer : virtual public BaseSerializer,
                                public SomethingWithUINTsSerializer {
public:
  SomethingMoreSerializer(void) { sm.uints = &swu; }
  ~SomethingMoreSerializer(void) {}
  void SetErrorCode(SomethingMore__Errors error_code) {
    sm.has_error_code = TRUE;
    sm.error_code = error_code;
  }
  uint16_t GetPduType(void) override { return PDU_SOMETHING_MORE; }
  size_t GetSerializedSize(void) override {
    return something_more__get_packed_size(&sm);
  }
  size_t Serialize(uint8_t *buf) override {
    return something_more__pack(&sm, buf);
  }

  SomethingMore sm = SOMETHING_MORE__INIT;
};

class SomethingMoreDeserializer : virtual public BaseDeserializer {
public:
  SomethingMoreDeserializer(void) {}
  ~SomethingMoreDeserializer(void) { DeserializeFree(); }
  bool Deserialize(size_t pdu_len, uint8_t *buf) override {
    sm = something_more__unpack(NULL, pdu_len, buf);
    return sm != NULL;
  }
  void DeserializeFree(void) override {
    if (sm != NULL)
      something_more__free_unpacked(sm, NULL);
  }

  SomethingMore *sm = NULL;
};

class EvenMoreSerializer : virtual public BaseSerializer {
public:
  explicit EvenMoreSerializer(void) = default;
  explicit EvenMoreSerializer(EvenMore__SomeEnum enum_value,
                              ProtobufCBinaryDataClass name,
                              ProtobufCBinaryDataClass value)
      : uints() {
    em.enum_value = enum_value;
    em.name = name;
    em.value = value;
  }
  ~EvenMoreSerializer(void) { free(s); }
  void AddUints(SomethingWithUINTsSerializer *uints) {
    this->uints.push_back(uints);
  }
  void SetS(eastl::string s) {
    size_t l = s.size();
    this->s = (char *)malloc(l + 1);
    memcpy(this->s, s.c_str(), l);
    this->s[l] = '\0';
  }
  uint16_t GetPduType(void) override { return PDU_EVEN_MORE; }
  size_t GetSerializedSize(void) override {
    em.s = s;
    em.n_uints = this->uints.size();
    if (em.n_uints > 0) {
      SomethingWithUINTs *out[em.n_uints];
      ConvertUintsVectorToCArray(out);
      return even_more__get_packed_size(&em);
    }
    return even_more__get_packed_size(&em);
  }
  size_t Serialize(uint8_t *buf) override {
    em.s = s;
    em.n_uints = this->uints.size();
    if (em.n_uints > 0) {
      SomethingWithUINTs *out[em.n_uints];
      ConvertUintsVectorToCArray(out);
      return even_more__pack(&em, buf);
    }
    return even_more__pack(&em, buf);
  }

  EvenMore em = EVEN_MORE__INIT;
  eastl::vector<SomethingWithUINTsSerializer *> uints;
  char *s = NULL;

  void ConvertUintsVectorToCArray(SomethingWithUINTs **out) {
    for (size_t i = 0; i < em.n_uints; ++i) {
      out[i] = &uints[i]->swu;
    }
    em.uints = out;
  }
};

class EvenMoreDeserializer : virtual public BaseDeserializer {
public:
  EvenMoreDeserializer(void) {}
  ~EvenMoreDeserializer(void) { DeserializeFree(); }
  bool Deserialize(size_t pdu_len, uint8_t *buf) override {
    em = even_more__unpack(NULL, pdu_len, buf);
    return em != NULL;
  }
  void DeserializeFree(void) override {
    if (em != NULL)
      even_more__free_unpacked(em, NULL);
  }

  EvenMore *em = NULL;
};
