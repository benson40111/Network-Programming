#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#define BUF_SIZE 64
#define PORT 4999
#pragma comment(lib, ""ws2_32.lib)

int main(){
    WSADATA wsd;
    SOCKET sServer;
    SOCKET sClient;
    SOCKADDR_IN addrServ;
    SOCKADDR_IN addrClient;
    int retVal;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sServer){
        printf("Socket Failed!\n");
        WSACleanup();
        return -1;
    }

    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(PORT);
    addrServ.sin_addr.s_addr = INADDR_ANY;

    retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
    if (SOCKET_ERROR == retVal){
        printf("Bind Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    retVal = listen(sServer, 1);
    if (SOCKET_ERROR == retVal){
        printf("Listen Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    printf("Ready to receive!\n");
    int addrClientlen = sizeof(addrClient);
    sClient = accept(sServer, (SOCKADDR FAR*)&addrClient, &addrClientlen);
    if (INVALID_SOCKET == sClient){
        printf("Accept Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    printf("(Using addrClient returned from accept())\nPeer IP address: %s Peer port: %d\n",
           inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

    struct sockaddr_storage addr_remote, addr_local;
    char ipstr[INET6_ADDRSTRLEN];
    int len_remote, len_local, port_remote, port_local;
    len_remote = sizeof(addr_remote);
    len_local = sizeof(addr_local);
    getpeername(sClient, (struct sockaddr*)&addr_remote, &len_remote);
    getsockname(sClient, (struct sockaddr*)&addr_local, &len_local);

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
    strcpy(buf, "Hello from server!\n");
    send(sClient, buf, strlen(buf), 0);

    system("pause");

    closesocket(sServer);
    closesocket(sClient);
    WSACleanup();
    return 0;
}
