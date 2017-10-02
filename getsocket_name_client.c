#include<stdio.h>
#define BUF_SIZE 64
#define PORT "4999"
#define IP "140.125.46.225"
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(){
    WSADATA wsd;
    SOCKET sHost;
    SOCKADDR_IN servAddr;
    char buf[BUF_SIZE];
    int retVal;

    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);
    int nServAddlen = sizeof(servAddr);

    retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));

    struct sockaddr_in local;
    int saSize = sizeof(local);
    memset(&local, 0, sizeof(local));
    getsockname(sHost, (struct sockaddr *)&local, &saSize);
    printf("The address of the connecting socket: %s:%d\n", inet_ntoa(local.sin_addr), ntohs(local.sin_port));

    system("pause");
    closesocket(sHost);
    WSACleanup();
    return 0;
}
