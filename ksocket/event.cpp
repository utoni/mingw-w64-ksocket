#include <ksocket/berkeley.h>
#include <ksocket/helper.hpp>

#include "event.hpp"

extern WSK_PROVIDER_NPI WskProvider;
extern CONST NPIID NPI_WSK_INTERFACE_ID;

KSocketEvent::KSocketEvent(void) : workQueue(), socketContext() {}

KSocketEvent::~KSocketEvent(void) {}

bool KSocketEvent::Setup(_In_ ULONG buffer_length) {
  ULONG i;

  for (i = 0; i < KSOCK_EVENT_OP_COUNT; i++) {
    socketContext.OpContext[i].SocketContext = &socketContext;
    socketContext.OpContext[i].Irp = IoAllocateIrp(1, FALSE);
    if (socketContext.OpContext[i].Irp == NULL) {
      return false;
    }

    if (buffer_length > 0) {
      socketContext.OpContext[i].DataBuffer = malloc(buffer_length);
      if (socketContext.OpContext[i].DataBuffer == NULL) {
        return false;
      }
      socketContext.OpContext[i].DataMdl =
          IoAllocateMdl(socketContext.OpContext[i].DataBuffer, buffer_length,
                        FALSE, FALSE, NULL);
      if (socketContext.OpContext[i].DataMdl == NULL) {
        return false;
      }
      MmBuildMdlForNonPagedPool(socketContext.OpContext[i].DataMdl);
      socketContext.OpContext[i].BufferLength = buffer_length;
    }
  }

  return true;
}

bool KSocketEvent::Listen(uint16_t port, uint16_t type, int protocol)
{
  NTSTATUS Status;
  WSK_EVENT_CALLBACK_CONTROL callbackControl;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  Status = KsCreateListenSocket(&socketContext.Socket, AF_INET, type, protocol ? protocol : IPPROTO_TCP);
  if (!NT_SUCCESS(Status))
  {
    return false;
  }

  Status = KsBind(socketContext.Socket, (PSOCKADDR)&addr);
  if (!NT_SUCCESS(Status))
  {
    return false;
  }

  callbackControl.NpiId = (PNPIID)&NPI_WSK_INTERFACE_ID;
  callbackControl.EventMask = WSK_EVENT_ACCEPT;
  Status = WskProvider.Dispatch->WskControlClient(
                            WskProvider.Client,
                            WSK_SET_STATIC_EVENT_CALLBACKS,
                            sizeof(callbackControl),
                            &callbackControl,
                            0,
                            NULL,
                            NULL,
                            NULL);

  return NT_SUCCESS(Status) ? true : false;
}

bool KSocketEvent::Close(void)
{
  NTSTATUS Status = KsCloseSocket(socketContext.Socket);

  socketContext.Socket = NULL;
  return NT_SUCCESS(Status) ? true : false;
}
