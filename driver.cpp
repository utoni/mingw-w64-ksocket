
extern "C" {
#include "berkeley.h"
#include "ksocket.h"
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

  NTSTATUS Status;

  //
  // Initialize KSOCKET.
  //

  Status = KsInitialize();

  if (!NT_SUCCESS(Status)) {
    return Status;
  }

  //
  // Client.
  // Perform HTTP request to http://httpbin.org/uuid
  //

  {
    int result;
    UNREFERENCED_PARAMETER(result);

    char send_buffer[] = "GET /uuid HTTP/1.1\r\n"
                         "Host: httpbin.org\r\n"
                         "Connection: close\r\n"
                         "\r\n";

    char recv_buffer[1024] = {};

    struct addrinfo hints = {};
    hints.ai_flags |= AI_CANONNAME;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    result = getaddrinfo("httpbin.org", "80", &hints, &res);

    int sockfd;
    sockfd = socket_connection(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = connect(sockfd, res->ai_addr, (int)res->ai_addrlen);
    result = send(sockfd, send_buffer, sizeof(send_buffer), 0);
    result = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
    recv_buffer[sizeof(recv_buffer) - 1] = '\0';

    DebuggerPrint("TCP client:\n%s\n", recv_buffer);

    closesocket(sockfd);
  }

  //
  // TCP server.
  // Listen on port 9095, wait for some message,
  // then send our buffer and close connection.
  //

  {
    int result;
    UNREFERENCED_PARAMETER(result);

    char send_buffer[] = "Hello from WSK!";
    char recv_buffer[1024] = {0};

    int server_sockfd = socket_listen(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9095);

    result = bind(server_sockfd, (struct sockaddr *)&addr, sizeof(addr));
    result = listen(server_sockfd, 1);

    socklen_t addrlen = sizeof(addr);
    int client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&addr, &addrlen);

    result = recv(client_sockfd, recv_buffer, sizeof(recv_buffer) - 1, 0);
    recv_buffer[sizeof(recv_buffer) - 1] = '\0';

    DebuggerPrint("TCP server:\n%s\n", recv_buffer);

    result = send(client_sockfd, send_buffer, sizeof(send_buffer), 0);

    closesocket(client_sockfd);
    closesocket(server_sockfd);
  }

  KsDestroy();

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.");
}
}
