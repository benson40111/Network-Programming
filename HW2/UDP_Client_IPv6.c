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
PCSTR IP = "127.0.0.1"; //local host

int main(){
    WSADATA wsd;
    SOCKET s;
    struct addrinfo hints, *res;
    char buf[BUF_SIZE]; //receive data buffer

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    getaddrinfo(IP, PORT, &hints, &res);

    s = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (s == INVALID_SOCKET){
        printf("Socket Failed!\n%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    ZeroMemory(buf, BUF_SIZE);
    strcpy(buf, "UDP");

    if (sendto(s, buf, BUF_SIZE, 0, res -> ai_addr, res -> ai_addrlen) == SOCKET_ERROR){
        printf("recvform() Failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    printf("Successful!\n");
    system("pause");

    closesocket(s);
    WSACleanup();
    return 0;
}
