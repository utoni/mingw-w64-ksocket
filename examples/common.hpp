#ifndef COMMON_H
#define COMMON_H 1

#include <ksocket/helper.hpp>

#include "examples/example.pb-c.h"

/* Could be also be done via protobuf */
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

#endif
