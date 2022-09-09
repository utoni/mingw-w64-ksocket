#include "berkeley.h"
#include "ksocket.h"
#include "protobuf-c/example.pb-c.h"
#include "wsk.h"

#include "common.hpp"

extern "C" {
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

  {
    uint8_t *buf = NULL;

    {
      SomethingMoreSerializer sms;
      sms.SetErrorCode(SOMETHING_MORE__ERRORS__SUCCESS);
      sms.SetId(0x12345678);
      sms.SetIpAddress(0xAAAAAAAA);
      sms.SetPortNum(0xBBBBBBBB);
      len = sms.GetSerializedSize();
      buf = (uint8_t *)malloc(len);
      if (buf == NULL) {
        return STATUS_UNSUCCESSFUL;
      }
      if (sms.Serialize(buf) != len) {
        DebuggerPrint("Packing failed.\n");
        free(buf);
        return STATUS_UNSUCCESSFUL;
      }

      DebuggerPrint("Packed Size: %zu\n", len);

      SomethingMoreDeserializer smd;
      if (smd.Deserialize(len, buf) == true && smd.sm->uints != NULL &&
          smd.sm->uints->id == 0x12345678 &&
          smd.sm->uints->ip_address == 0xAAAAAAAA &&
          smd.sm->uints->port_num == 0xBBBBBBBB) {
        DebuggerPrint("Success!\n");
      }
    }

    free(buf);
  }

  {
    EvenMoreSerializer ems(EVEN_MORE__SOME_ENUM__FIRST,
                           {0xde, 0xad, 0xc0, 0xde}, {0xde, 0xad, 0xc0, 0xde});
    SomethingWithUINTsSerializer sws[3];

    sws[0].SetId(0xdeadc0de);
    sws[1].SetIpAddress(0xdeadbeef);
    sws[2].SetPortNum(0xcafecafe);

    ems.SetS("This is a zero-terminated String!");
    ems.AddUints(&sws[0]);
    ems.AddUints(&sws[1]);
    ems.AddUints(&sws[2]);

    len = ems.GetSerializedSize();
    uint8_t tmp[len];
    if (ems.Serialize(tmp) != len) {
      DebuggerPrint("Packing failed.\n");
    }
    DebuggerPrint("Packed Size: %zu\n", len);

    EvenMoreDeserializer emd;
    if (emd.Deserialize(len, tmp) == true && emd.em->n_uints == 3 &&
        emd.em->uints != NULL && emd.em->name.len > 0 &&
        emd.em->name.data != NULL && emd.em->value.len > 0 &&
        emd.em->value.data != NULL && emd.em->s != NULL) {
      if (emd.em->enum_value == EVEN_MORE__SOME_ENUM__FIRST ||
          emd.em->uints[0]->has_id == TRUE ||
          emd.em->uints[0]->id == 0xdeadc0de ||
          emd.em->uints[1]->has_ip_address == TRUE ||
          emd.em->uints[1]->ip_address == 0xdeadbeef ||
          emd.em->uints[2]->has_port_num == TRUE ||
          emd.em->uints[2]->port_num == 0xcafecafe) {
        DebuggerPrint("Success!\n");
      }
      DebuggerPrint("Deserialized String: '%s'\n", emd.em->s);
    }
  }

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.");
}
}
