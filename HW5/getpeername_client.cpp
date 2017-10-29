#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#define BUF_SIZE 64
#define PORT 4999
#define IP "127.0.0.1"
#pragma comment(lib, ""ws2_32.lib)

int main(){
    WSADATA wsd;
    SOCKET sHost;
    SOCKADDR_IN servAddr;
    int retVal;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sHost){
        printf("Socket Failed!\n");
        WSACleanup();
        return -1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr(IP);

    printf("(Get from servAddr)\nPeer IP address: %s Peer port: %d\n",
           inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

    retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
    if (SOCKET_ERROR == retVal){
        printf("Connect Failed!\n");
        closesocket(sHost);
        WSACleanup();
        return -1;
    }

    struct sockaddr_storage addr_remote, addr_local;
    char ipstr[INET6_ADDRSTRLEN];
    int len_remote, len_local, port_remote, port_local;
    len_remote = sizeof(addr_remote);
    len_local = sizeof(addr_local);
    getpeername(sHost, (struct sockaddr*)&addr_remote, &len_remote);
    getsockname(sHost, (struct sockaddr*)&addr_local, &len_local);

    if (addr_remote.ss_family == AF_INET){
        struct sockaddr_in *s_remote = (struct sockaddr_in*)&addr_remote;
        port_remote = ntohs(s_remote->sin_port);
        inet_ntop(AF_INET, &s_remote->sin_addr, ipstr, sizeof(ipstr));
    }
    else{
        struct sockaddr_in6 *s_remote = (struct sockaddr_in6*)&addr_remote;
        port_remote = ntohs(s_remote->sin6_port);
        inet_ntop(AF_INET6, &s_remote->sin6_addr, ipstr, sizeof(ipstr));
    }

    printf("(Using getpeername())\nPeer IP address: %s ", ipstr);
    printf("Peer port: %d\n", port_remote);

    ZeroMemory(ipstr, INET6_ADDRSTRLEN);
    if (addr_local.ss_family == AF_INET){
        struct sockaddr_in *s_local = (struct sockaddr_in*)&addr_local;
        port_local = ntohs(s_local->sin_port);
        inet_ntop(AF_INET, &s_local->sin_addr, ipstr, sizeof(ipstr));
    }
    else{
        struct sockaddr_in6 *s_local = (struct sockaddr_in6*)&addr_local;
        port_local = ntohs(s_local->sin6_port);
        inet_ntop(AF_INET6, &s_local->sin6_addr, ipstr, sizeof(ipstr));
    }

    printf("\n(Using getsockname())\nLocal IP address: %s ", ipstr);
    printf("Local port: %d\n", port_local);

    char buf[64];
    ZeroMemory(buf, 64);
    recv(sHost, buf, 64, 0);
    printf("%s\n", buf);

    system("pause");

    closesocket(sHost);
    WSACleanup();
    return 0;
}
