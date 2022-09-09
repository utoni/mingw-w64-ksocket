#include "berkeley.h"
#include "ksocket.h"
#include "examples/example.pb-c.h"
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

  NTSTATUS Status;

  DebuggerPrint("Hi.\n");
  Status = KsInitialize();

  if (!NT_SUCCESS(Status)) {
    return Status;
  }

  int server_sockfd = socket_listen(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(9095);

  int result = bind(server_sockfd, (struct sockaddr *)&addr, sizeof(addr));
  if (result != 0) {
    DebuggerPrint("TCP server bind failed\n");
    return STATUS_FAILED_DRIVER_ENTRY;
  }

  result = listen(server_sockfd, 1);
  if (result != 0) {
    DebuggerPrint("TCP server listen failed\n");
    return STATUS_FAILED_DRIVER_ENTRY;
  }

  socklen_t addrlen = sizeof(addr);
  int client_sockfd = accept(server_sockfd, (struct sockaddr *)&addr, &addrlen);
  if (client_sockfd < 0) {
    DebuggerPrint("TCP accept failed\n");
    return STATUS_FAILED_DRIVER_ENTRY;
  }

  int iResult;
  SocketBuffer<1024> sb_send, sb_recv;

  do {
    bool ok = false;
    RECV_PDU_BEGIN(client_sockfd, sb_recv, iResult, pdu_type, pdu_len) {
      DebuggerPrint("PDU type/len: %u/%u\n", pdu_type, pdu_len);
      switch ((enum PduTypes)pdu_type) {
      case PDU_SOMETHING_WITH_UINTS: {
        SomethingWithUINTsDeserializer swud;
        if ((ok = swud.Deserialize(pdu_len, sb_recv.GetStart())) == true) {
          SomethingWithUINTsSerializer swus;
          if (swud.swu->has_id == TRUE) {
            DebuggerPrint("Id: 0x%X\n", swud.swu->id);
            swus.SetId(swud.swu->id + 1);
          }
          ok = sb_send.AddPdu(swus);
        }
        break;
      }
      case PDU_SOMETHING_MORE: {
        SomethingMoreDeserializer smd;
        if ((ok = smd.Deserialize(pdu_len, sb_recv.GetStart())) == true) {
          SomethingMoreSerializer sms;
          if (smd.sm->has_error_code == TRUE) {
            DebuggerPrint("Error Code: %u\n", smd.sm->error_code);
          }
          if (smd.sm->uints->has_id == TRUE) {
            DebuggerPrint("Id: 0x%X\n", smd.sm->uints->id);
            sms.SetId(smd.sm->uints->id + 1);
          }
          sms.SetErrorCode(SOMETHING_MORE__ERRORS__SUCCESS);
          sms.SetIpAddress(0xCCCCCCCC);
          sms.SetPortNum(0xDDDDDDDD);
          ok = sb_send.AddPdu(sms);
        }
        break;
      }
      case PDU_EVEN_MORE: {
        EvenMoreDeserializer emd;
        if ((ok = emd.Deserialize(pdu_len, sb_recv.GetStart())) == true) {
          DebuggerPrint("EnumValue: %d\n", emd.em->enum_value);
          if (emd.em->s != NULL) {
            DebuggerPrint("String: '%s'\n", emd.em->s);
          }
          EvenMoreSerializer ems;
          SomethingWithUINTsSerializer swus;
          swus.SetId(0xDEADDEAD);
          ems.SetS("Hi userspace!");
          ems.AddUints(&swus);
          ok = sb_send.AddPdu(ems);
        }
        break;
      }
      }
    }
    RECV_PDU_END(sb_recv, pdu_len);

    if (ok == true) {
      SEND_ALL(client_sockfd, sb_send, iResult);
      if (iResult == SOCKET_ERROR || iResult == 0) {
        DebuggerPrint("send failed\n");
        break;
      }
    } else {
      DebuggerPrint("Serialization/Deserialization failed\n");
      break;
    }
  } while (iResult != SOCKET_ERROR && iResult > 0);

  DebuggerPrint("Client gone.\n") closesocket(client_sockfd);
  closesocket(server_sockfd);
  KsDestroy();

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.\n");
}
}
