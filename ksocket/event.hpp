#pragma once
#include <DriverThread.hpp>
#include <ksocket/ksocket.h>
#include <ksocket/wsk.h>
#include <ntddk.h>

// Maximum number of operations that can be outstanding on a socket at any time 
#define KSOCK_EVENT_OP_COUNT 2

struct KSOCKET_EVENT_OP_CONTEXT;
struct KSOCKET_EVENT_CONTEXT;

typedef VOID (* KSOCKET_EVENT_OP_HANDLER_FN)(_In_ KSOCKET_EVENT_OP_CONTEXT *SocketOpContext);

// Structure that represents the context for a WSK socket operation.
struct KSOCKET_EVENT_OP_CONTEXT {
    SLIST_ENTRY QueueEntry;
    KSOCKET_EVENT_OP_HANDLER_FN OpHandler;
    KSOCKET_EVENT_CONTEXT *SocketContext;
    PIRP Irp;
    _Field_size_bytes_part_(BufferLength,DataLength) PVOID  DataBuffer;
    PMDL   DataMdl;
    SIZE_T BufferLength;
    SIZE_T DataLength;
};

// Structure that represents the context for a WSK socket.
struct KSOCKET_EVENT_CONTEXT {
    PKSOCKET Socket;

    // Socket is being closed.
    BOOLEAN Closing;

    // Peer has gracefully disconnected its half of the connection and we are
    // about to disconnect (or have disconnected) our half.
    BOOLEAN Disconnecting;

    // Stop accepting incoming connections. Valid for listening sockets only.
    BOOLEAN StopListening;

    // Embedded array of contexts for outstanding operations on the socket.
    // Note that operation contexts could also be allocated separately. This
    // sample preallocates a fixed number of operation contexts along with
    // the socket context for a new socket.
    KSOCKET_EVENT_OP_CONTEXT OpContext[KSOCK_EVENT_OP_COUNT];
};

class KSocketEvent {
public:
    KSocketEvent(void);
    ~KSocketEvent(void);
    bool Setup(_In_ ULONG buffer_length);
    bool Listen(uint16_t port, uint16_t type, int protocol);
    bool Close(void);

private:
    DriverThread::WorkQueue workQueue;
    struct KSOCKET_EVENT_CONTEXT socketContext;
};
