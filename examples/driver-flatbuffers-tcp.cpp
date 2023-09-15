#include <ksocket/berkeley.h>
#include <ksocket/helper.hpp>
#include <ksocket/ksocket.hpp>
#include <ksocket/ksocket.h>
#include <ksocket/wsk.h>

#include "apiwrapper_builder.h"
#include "apiwrapper_reader.h"
#include "apiwrapper_verifier.h"

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

  KSocketBuffer buf;
  buf.insert(buf.end(), static_cast<uint16_t>(0x1122));
  buf.insert(buf.end(), static_cast<uint32_t>(0xFFFFFFFF));
  buf.insert(buf.end(), "AAAAAAAA");
  DebuggerPrint("HEX: %s\n", buf.toHex().c_str());

  DebuggerPrint("Hi.\n");
  Status = KsInitialize();

  if (!NT_SUCCESS(Status)) {
    return Status;
  }

  int server_sockfd = socket_listen(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(9096);

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
    RECV_PDU_BEGIN(client_sockfd, sb_recv, iResult, pdu_type, pdu_len) {
        DebuggerPrint("PDU type/len: %u/%u\n", pdu_type, pdu_len);
        if (pdu_type == 0) {
          int ret = FunctionAddresses_verify_as_root(sb_recv.GetStart(), pdu_len);

          if (ret == 0) {
            FunctionAddresses_table_t fnaddr = FunctionAddresses_as_root(sb_recv.GetStart());

            if (!fnaddr) {
              DebuggerPrint("%s\n", "FunctionAddresses not available!");
            } else {
              flatbuffers_string_vec_t names = FunctionAddresses_names(fnaddr);
              size_t name_size = flatbuffers_string_vec_len(names);

              DebuggerPrint("Length of names vector: %zu\n", name_size);
            }
          } else {
            DebuggerPrint("Flatbuffer verification failed with %d: %s\n", ret, flatcc_verify_error_string(ret));
          }
        } else {
          DebuggerPrint("%s\n", "PDU type not supported!");
        }
    }
    RECV_PDU_END(sb_recv, pdu_len);
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
