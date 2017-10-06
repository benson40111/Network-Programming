/*-----Important!!!-----*/
/*if error occur, using cmd and enter command below:
    CD C:\Program Files (X86)\CodeBlocks\MinGW\bin
    gcc your_file_path -lws2_32
    You will see a output file which name is a.exe
*/
#define _WIN32_WINNT 0x501
#include<stdio.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib");
#define BUF_SIZE 64
PCSTR PORT = "5000";

int main(){
    WSADATA wsd;
    SOCKET s;
    struct addrinfo hints, *res;
    struct sockaddr_storage addrClient;
    char buf[BUF_SIZE]; //receive data buffer

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &res);

    s = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (s == INVALID_SOCKET){
        printf("Socket Failed!\n%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    int nErrCode;   //Return Value
    int nBufLen;
    int nOptlen = sizeof(nBufLen);
    nErrCode = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&nBufLen, &nOptlen);
    if (SOCKET_ERROR == nErrCode){
        printf("Error!\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    nBufLen *= 10;
    nErrCode = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&nBufLen, nOptlen);
    if (SOCKET_ERROR == nErrCode){
        printf("Processing Error!\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    int uiNewRcvBuf;
    nErrCode = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&uiNewRcvBuf, &nOptlen);
    if (SOCKET_ERROR == nErrCode || uiNewRcvBuf != nBufLen){
        printf("Error!\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    //Bind Server
    if (bind(s, res -> ai_addr, res -> ai_addrlen) == SOCKET_ERROR){
        printf("Bind Failed!\n");
        closesocket(s);
        WSACleanup();
        return -1;
    }

    int nClientLen = sizeof(addrClient);
    ZeroMemory(buf, BUF_SIZE);
    printf("Ready to receive data!\n");

    if (recvfrom(s, buf, BUF_SIZE, 0, (SOCKADDR*)&addrClient, &nClientLen) == SOCKET_ERROR){
        printf("recvform() Failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    printf("%s\n", buf);    //print to screen
    system("pause");

    closesocket(s);
    WSACleanup();
    return 0;
}
