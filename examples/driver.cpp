#include <ksocket/berkeley.h>
#include <ksocket/ksocket.h>
#include <ksocket/utils.h>
#include <ksocket/wsk.h>

#include <ksocket/ksocket.hpp>

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

  const char send_buffer[] = "GET /uuid HTTP/1.1\r\n"
                             "Host: httpbin.org\r\n"
                             "Connection: close\r\n"
                             "\r\n";

  {
    int result;
    UNREFERENCED_PARAMETER(result);

    char recv_buffer[1024] = {};

    struct addrinfo hints = {};
    hints.ai_flags |= AI_CANONNAME;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    result = getaddrinfo("httpbin.org", "80", &hints, &res);

    int sockfd;
    sockfd = socket_connection(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
      DebuggerPrint("TCP client socket_connection failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    result = connect(sockfd, res->ai_addr, (int)res->ai_addrlen);
    if (result != 0) {
      DebuggerPrint("TCP client connect failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    result = send(sockfd, send_buffer, sizeof(send_buffer) - 1, 0);
    if (result <= 0) {
      DebuggerPrint("TCP client send failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    result = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
    if (result <= 0) {
      DebuggerPrint("TCP client recv failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    } else {
      DebuggerPrint("TCP client:\n%.*s\n", result, recv_buffer);
    }

    closesocket(sockfd);
  }

  {
    KStreamClientIp4 tcp4_client = KStreamClientIp4();

    if (!tcp4_client.setup()) {
      DebuggerPrint("KStreamClientIp4 setup() failed: %d\n",
                    tcp4_client.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    if (!tcp4_client.connect("httpbin.org", "80")) {
      DebuggerPrint("KStreamClientIp4 connect() failed: %d\n",
                    tcp4_client.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    DebuggerPrint("%s\n", "KStreamClientIp4 connected!");

    tcp4_client.getSendBuffer().insert(tcp4_client.getSendBuffer().end(),
                                       send_buffer, sizeof(send_buffer) - 1);
    if (!tcp4_client.send()) {
      DebuggerPrint("KStreamClientIp4 send() failed: %d\n",
                    tcp4_client.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    if (!tcp4_client.recv()) {
      DebuggerPrint("KStreamClientIp4 recv() failed: %d\n",
                    tcp4_client.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    DebuggerPrint("KStreamClientIp4 data received:\n%s\n",
                  tcp4_client.getRecvBuffer().to_string().c_str());
    DebuggerPrint("KStreamClientIp4 consuming %zu bytes\n",
                  tcp4_client.getRecvBuffer().size());
    tcp4_client.getRecvBuffer().consume();
    DebuggerPrint("KStreamClientIp4 receive buffer size: %zu\n",
                  tcp4_client.getRecvBuffer().size());

    DebuggerPrint("%s\n", "KStreamClientIp4 finished.");
  }

  //
  // TCP server.
  // Listen on port 9095, wait for some message,
  // then send our buffer and close connection.
  //

  {
    int result;

    char send_buffer[] = "Hello from WSK!";
    char recv_buffer[1024] = {0};

    int server_sockfd = socket_listen(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9095);

    result = bind(server_sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (result != 0) {
      DebuggerPrint("TCP server bind failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    result = listen(server_sockfd, 1);
    if (result != 0) {
      DebuggerPrint("TCP server listen failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    DebuggerPrint(
        "%s\n",
        "TCP server is waiting for the user to start userspace_client.exe");

    socklen_t addrlen = sizeof(addr);
    int client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&addr, &addrlen);
    if (client_sockfd < 0) {
      DebuggerPrint("TCP accept failed\n");
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    result = recv(client_sockfd, recv_buffer, sizeof(recv_buffer) - 1, 0);
    if (result > 0) {
      DebuggerPrint("TCP server received: \"%.*s\"\n", result, recv_buffer);
    } else {
      DebuggerPrint("TCP server recv failed\n");
    }

    result = send(client_sockfd, send_buffer, sizeof(send_buffer), 0);

    // Wait for the client to terminate the connection.
    do {
    } while (recv(client_sockfd, recv_buffer, sizeof(recv_buffer) - 1, 0) > 0);

    closesocket(client_sockfd);
    closesocket(server_sockfd);
  }

  {
    KStreamServerIp4 tcp4_server = KStreamServerIp4();

    if (!tcp4_server.setup()) {
      DebuggerPrint("KStreamServerIp4 setup() failed: %d\n",
                    tcp4_server.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    if (!tcp4_server.bind(9095)) {
      DebuggerPrint("KStreamServerIp4 bind() failed: %d\n",
                    tcp4_server.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    if (!tcp4_server.listen()) {
      DebuggerPrint("KStreamServerIp4 bind() failed: %d\n",
                    tcp4_server.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }

    DebuggerPrint("%s\n", "KStreamServerIp4 listening for incomining "
                          "connections (run userspace_client.exe again)..");

    const auto &accept_fn = [](KAcceptedSocket &ka) {
      const auto &remote = ka.getRemote();

      if (remote.addr_used != 4) {
        return false;
      }
      DebuggerPrint("KStreamServerIp4 client connected: %s\n",
                    remote.to_string().c_str());

      if (!ka.recv()) {
        DebuggerPrint("KStreamServerIp4 recv failed: %d\n", ka.getLastError());
        return false;
      }
      DebuggerPrint("KStreamServerIp4 received %zu bytes: \"%s\"\n",
                    ka.getRecvBuffer().size(),
                    ka.getRecvBuffer().to_string().c_str());
      ka.getRecvBuffer().consume();

      ka.getSendBuffer().insert_string(ka.getSendBuffer().end(),
                                       "KStreamServerIp4 says hello!");
      if (!ka.send()) {
        DebuggerPrint("KStreamServerIp4 send failed: %d\n", ka.getLastError());
        return false;
      }
      ka.getSendBuffer().consume();

      // Wait for the connection termination.
      ka.recv();

      return true;
    };
    if (!tcp4_server.accept(accept_fn)) {
      DebuggerPrint("KStreamServerIp4 accept() failed: %d\n",
                    tcp4_server.getLastError());
      return STATUS_FAILED_DRIVER_ENTRY;
    }
    DebuggerPrint("KStreamServerIp4 done\n");
  }

  KsDestroy();

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.");
}
}
