#include <stdio.h>
#include <stdlib.h> // Needed for _wtoi
#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.hpp"
#include "protobuf-c/example.pb-c.h"

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

  uint32_t start_id = 0x12345678;
  for (size_t i = 0; i < 256; ++i) {
    SocketBuffer<1024> sb_send, sb_recv;
    SomethingMoreSerializer sms;

    sms.SetErrorCode(SOMETHING_MORE__ERRORS__SUCCESS);
    sms.SetId(start_id++);
    sms.SetIpAddress(0xAAAAAAAA);
    sms.SetPortNum(0xBBBBBBBB);

    if (!sb_send.AddPdu(sms)) {
      wprintf(L"Serialization failed\n");
      break;
    }

    SEND_ALL(sock, sb_send, iResult);
    if (iResult == SOCKET_ERROR || iResult == 0) {
      wprintf(L"send failed with error: %d\n", WSAGetLastError());
      break;
    }

    RECV_PDU_BEGIN(sock, sb_recv, iResult, pdu_type, pdu_len) {
      wprintf(L"PDU type/len: %u/%u\n", pdu_type, pdu_len);
      switch ((enum PduTypes)pdu_type) {
      case PDU_SOMETHING_WITH_UINTS: {
        break;
      }
      case PDU_SOMETHING_MORE: {
        SomethingMoreDeserializer smd;
        if (smd.Deserialize(pdu_len, sb_recv.GetStart()) == true &&
            smd.sm->uints != NULL && smd.sm->uints->has_id == TRUE &&
            smd.sm->uints->has_ip_address == TRUE &&
            smd.sm->uints->has_port_num == TRUE)
          wprintf(L"Id: 0x%X, IpAddress: 0x%X, PortNum: 0x%X\n",
                  smd.sm->uints->id, smd.sm->uints->ip_address,
                  smd.sm->uints->port_num);
        break;
      }
      case PDU_EVEN_MORE: {
        break;
      }
      }
    }
    RECV_PDU_END(sb_recv, pdu_len);

    ////////////////////////////////////////////////////////

    EvenMoreSerializer ems(EVEN_MORE__SOME_ENUM__FIRST,
                           {0xde, 0xad, 0xc0, 0xde}, {0xde, 0xad, 0xc0, 0xde});
    SomethingWithUINTsSerializer swus[3];

    swus[0].SetId(0xdeadc0de);
    swus[1].SetIpAddress(0xdeadbeef);
    swus[2].SetPortNum(0xcafecafe);

    ems.SetS("This is a zero-terminated String!");
    ems.AddUints(&swus[0]);
    ems.AddUints(&swus[1]);
    ems.AddUints(&swus[2]);

    if (!sb_send.AddPdu(ems)) {
      wprintf(L"Serialization failed\n");
      break;
    }

    SEND_ALL(sock, sb_send, iResult);
    if (iResult == SOCKET_ERROR || iResult == 0) {
      wprintf(L"send failed with error: %d\n", WSAGetLastError());
      break;
    }

    RECV_PDU_BEGIN(sock, sb_recv, iResult, pdu_type, pdu_len) {
      wprintf(L"PDU type/len: %u/%u\n", pdu_type, pdu_len);
      switch ((enum PduTypes)pdu_type) {
      case PDU_SOMETHING_WITH_UINTS: {
        break;
      }
      case PDU_SOMETHING_MORE: {
        break;
      }
      case PDU_EVEN_MORE: {
        EvenMoreDeserializer emd;
        if (emd.Deserialize(pdu_len, sb_recv.GetStart()) == true) {
          wprintf(L"EnumValue: %d\n", emd.em->enum_value);
          if (emd.em->s != NULL)
            wprintf(L"String: '%s'\n", emd.em->s);
        }
        break;
      }
      }
    }
    RECV_PDU_END(sb_recv, pdu_len);
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
