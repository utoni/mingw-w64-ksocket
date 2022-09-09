#include <stdio.h>
#include <stdlib.h> // Needed for _wtoi
#include <winsock2.h>
#include <ws2tcpip.h>

int main(int argc, char **argv) {
  WSADATA wsaData = {};
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  if (iResult != 0) {
    wprintf(L"WSAStartup failed: %d\n", iResult);
    return 1;
  }

  SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == INVALID_SOCKET) {
    wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
  } else {
    wprintf(L"socket function succeeded\n");
  }

  sockaddr_in clientService;
  clientService.sin_family = AF_INET;
  clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
  clientService.sin_port = htons(9095);

  do {
    iResult = connect(sock, (SOCKADDR *)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
      wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
      Sleep(1000);
    }
  } while (iResult == SOCKET_ERROR);

  wprintf(L"Connected to server.\n");

  char sendbuf[] = "Hello Driver, greetings from userspace.";
  iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
  if (iResult == SOCKET_ERROR) {
    wprintf(L"send failed with error: %d\n", WSAGetLastError());
    closesocket(sock);
    WSACleanup();
    return 1;
  }

  {
    char recvbuf[1024];

    iResult = recv(sock, recvbuf, 1024, 0);
    if (iResult > 0) {
      wprintf(L"Bytes received: %d\n", iResult);
      wprintf(L"Data received: %.*s\n", iResult, recvbuf);
    } else if (iResult == 0) {
      wprintf(L"Connection closed by remote\n");
    } else if (WSAGetLastError() != WSAECONNRESET) {
      wprintf(L"recv failed: %d\n", WSAGetLastError());
    }
  };
  wprintf(L"Closing Connection ..\n");

  iResult = closesocket(sock);
  if (iResult == SOCKET_ERROR) {
    wprintf(L"closesocket function failed with error: %ld\n",
            WSAGetLastError());
    WSACleanup();
    return 1;
  }

  WSACleanup();

  system("pause");

  return 0;
}
