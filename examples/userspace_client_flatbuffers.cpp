#include <stdio.h>
#include <stdlib.h> // Needed for _wtoi
#include <winsock2.h>
#include <ws2tcpip.h>

#include <ksocket/ksocket.hpp>
#include <ksocket/helper.hpp>

#include "apiwrapper_builder.h"
#include "apiwrapper_reader.h"
#include "apiwrapper_verifier.h"

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
  clientService.sin_port = htons(9096);

  do {
    iResult = connect(sock, (SOCKADDR *)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
      wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
      Sleep(1000);
    }
  } while (iResult == SOCKET_ERROR);

  wprintf(L"Connected to server.\n");

  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  for (size_t i = 0; i < 256; ++i) {
    FunctionAddresses_start_as_root(&builder);
    FunctionAddresses_names_add(&builder, flatbuffers_string_create_str(&builder, "A"));
    FunctionAddresses_names_add(&builder, flatbuffers_string_create_str(&builder, "B"));
    FunctionAddresses_names_add(&builder, flatbuffers_string_create_str(&builder, "C"));
    FunctionAddresses_end_as_root(&builder);

    KSocketBuffer buffer;
    void *buf;
    size_t siz;
    buf = flatcc_builder_finalize_aligned_buffer(&builder, &siz);
    (void)buf;
    uint8_t a[] = {0x41,0x41,0x41};
    buffer.insert_u16(buffer.begin(), 65535);
    buffer.insert_bytebuffer(buffer.begin(), a, 3);
  }

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
