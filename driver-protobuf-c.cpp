
extern "C" {
#include "berkeley.h"
#include "ksocket.h"
#include "protobuf-c/example.pb-c.h"
#include "wsk.h"

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

#define DebuggerPrint(...)                                                     \
  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__);

NTSTATUS
NTAPI
DriverEntry(_In_ PDRIVER_OBJECT DriverObject,
            _In_ PUNICODE_STRING RegistryPath) {
  UNREFERENCED_PARAMETER(DriverObject);
  UNREFERENCED_PARAMETER(RegistryPath);

  size_t len = 0;
  uint8_t *buf = NULL;

  {
    SomethingMore sm = SOMETHING_MORE__INIT;
    SomethingWithUINTs swu = SOMETHING_WITH_UINTS__INIT;

    sm.error_code = SOMETHING_MORE__ERRORS__SUCCESS;
    sm.uints = &swu;
    swu.has_id = TRUE;
    swu.id = 0x12345678;
    swu.has_ip_address = TRUE;
    swu.ip_address = 0xAAAAAAAA;
    swu.has_port_num = TRUE;
    swu.port_num = 0xBBBBBBBB;

    len = something_more__get_packed_size(&sm);
    buf = (uint8_t *)malloc(len);
    if (something_more__pack(&sm, buf) != len) {
      DebuggerPrint("Packing failed.\n");
    }
  }

  DebuggerPrint("Packed Size: %zu\n", len);

  {
    SomethingMore *smth = something_more__unpack(NULL, len, buf);
    if (smth != NULL && smth->uints != NULL && smth->uints->id == 0x12345678 &&
        smth->uints->ip_address == 0xAAAAAAAA &&
        smth->uints->port_num == 0xBBBBBBBB) {
      DebuggerPrint("Success!\n");
    }
    DebuggerPrint("id: %x, ip_address: %x, port_num: %x\n", smth->uints->id,
                  smth->uints->ip_address, smth->uints->port_num);
    something_more__free_unpacked(smth, NULL);
  }

  free(buf);

  {
    EvenMore em = EVEN_MORE__INIT;
    SomethingWithUINTs swu[] = {SOMETHING_WITH_UINTS__INIT,
                                SOMETHING_WITH_UINTS__INIT,
                                SOMETHING_WITH_UINTS__INIT};
    SomethingWithUINTs *swu_vec[] = {&swu[0], &swu[1], &swu[2]};
    uint8_t bin[] = {0xde, 0xad, 0xc0, 0xde};
    ProtobufCBinaryData pbin = {.len = sizeof(bin), .data = bin};
    char str[] = "This is a zero-terminated String!";

    em.enum_value = EVEN_MORE__SOME_ENUM__FIRST;
    swu[0].has_id = TRUE;
    swu[0].id = 0xdeadc0de;
    swu[1].has_ip_address = TRUE;
    swu[1].ip_address = 0xdeadbeef;
    swu[2].has_port_num = TRUE;
    swu[2].port_num = 0xcafecafe;
    em.n_uints = sizeof(swu) / sizeof(swu[0]);
    em.uints = swu_vec;
    em.name = pbin;
    em.value = pbin;
    em.s = str;

    len = even_more__get_packed_size(&em);
    uint8_t tmp[len];
    if (even_more__pack(&em, tmp) != len) {
      DebuggerPrint("Packing failed.\n");
    }
    DebuggerPrint("Packed Size: %zu\n", len);

    EvenMore *result = even_more__unpack(NULL, len, tmp);
    if (result != NULL && result->n_uints == 3 && result->uints != NULL &&
        result->name.len > 0 && result->name.data != NULL &&
        result->value.len > 0 && result->value.data != NULL &&
        result->s != NULL) {
      if (result->enum_value != EVEN_MORE__SOME_ENUM__FIRST ||
          result->uints[0]->has_id != TRUE ||
          result->uints[0]->id != 0xdeadc0de ||
          result->uints[1]->has_ip_address != TRUE ||
          result->uints[1]->ip_address != 0xdeadbeef ||
          result->uints[1]->has_port_num != TRUE ||
          result->uints[2]->port_num != 0xcafecafe) {
        DebuggerPrint("Success!\n");
      }
      DebuggerPrint("Deserialized String: '%s'\n", result->s);
    }
    even_more__free_unpacked(result, NULL);
  }

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.");
}
}
