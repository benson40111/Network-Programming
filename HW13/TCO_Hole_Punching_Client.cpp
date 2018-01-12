#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<string.h>
#include<time.h>
#define BUF_SIZE 64
#define PORT 5000
#define IP "140.125.45.130"

#pragma comment(lib, "ws2_32.lib")

int main(){
    WSADATA wsd;
    SOCKET s, s_HolePunching_listen, s_HolePunching_connect, sClient;
    SOCKADDR_IN servAddr, localAddr, peerAddr, clientAddr, test;
    bool flag = true;
    char buf_send[BUF_SIZE], buf_recv1[BUF_SIZE], buf_recv2[BUF_SIZE];
    int peerPort, retVal;
    int sLen = sizeof(SOCKADDR_IN);
    unsigned long u1 = 1;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        printf("WSAStartup failed!\n");
        return 1;
    }

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET){
        printf("socket() failed; %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (setsockopt(s, SQL_SOCKRT, SO_REUSEADDR, (const char*)&flag, sizeof(BOOL)) == -1)
        printf("setsockopt on s: error\n");

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr(IP);

    printf("連線 Rendezvous Server...\n");
    retVal = connect(s, (SOCKADDR*)&servAddr, sizeof(servAddr));
    if (retVal == SOCKET_ERROR){
        printf("connect() failed!\n");
        WSACleanup();
        return 1;
    }

    LPHOSTENT hostEntry;
    char hostname[BUF_SIZE];
    gethostname(hostname, BUF_SIZE);
    hostEntry = gethostbyname(hostname);
    struct in_addr ** addr_list;
    addr_list = (struct in_addr**)hostEntry->h_addr_list;

    struct sockaddr_in local;
    memset(&local, 0, sLen);
    getsockname(s, (struct sockaddr*)&local, &sLen);
    int local_port = ntohs(local.sin_port);

    ZeroMemory(buf_send, BUF_SIZE);
    strcpy(buf_send, "(Registration message)");
    if (send(s, buf_send, BUF_SIZE, 0) == SOCKET_ERROR)
        printf("send() failed: %d\n", WSAGetLastError());
    printf("已發送註冊訊息到 Rendezvous Server.\n");

    char str_local_endpoint[BUF_SIZE];
    memset(&str_local_endpoint, 0, BUF_SIZE);
    strcpy(str_local_endpoint, inet_ntoa(*addr_list[0]));
    strcat(str_local_endpoint, ":");
    memset(&buf_recv1, 0, BUF_SIZE);
    sprintf(buf_recv1, "%d", local_port);
    strcat(str_local_endpoint, buf_recv1);

    printf("接收從Rendezvous Server送來之 Peer的endpoint(IP:Port), 並將之寫入Socket address 'PeerAddr'");
    ZeroMemory(buf_recv1, BUF_SIZE);
    if (recv(s, buf_recv1, BUF_SIZE, 0) == SOCKET_ERROR)
        printf("recv() failed: %d\n", WSAGetLastError());
    memset(&buf_recv2, 0, BUF_SIZE);
    strcpy(buf_recv2, buf_recv1);
    strtok(buf_recv1, ":");
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_addr.s_addr = inet_addr(buf_recv1);
    int ip_len = strlen(buf_recv1);
    memset(&buf_recv1, 0, BUF_SIZE);
    strcpy(buf_recv1, buf_recv2 + ip_len + 1);
    sscanf(buf_recv1, "%d", &peerPort);
    peerAddr.sin_port = htons(peerPort);
    printf("\t--> %s\n", buf_recv2);

    s_HolePunching_connect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_HolePunching_connect == INVALID_SOCKET){
        printf("socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (setsockopt(s_HolePunching_connect, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(BOOL) == -1)
        printf("setsockopt on s_HolePunching_connect: error\n");

    if (bind(s_HolePunching_connect, (SOCKADDR*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR){
        printf("bind() failed: %d\n", WSAGetLastError());
        closesocket(s_HolePunching_listen);
        WSACleanup();
        return 1;
    }

    s_HolePunching_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s_HolePunching_listen == INVALID_SOCKET) {
        printf("socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (setsockopt(s_HolePunching_listen, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(BOOL)) == -1)
        printf("setsockopt on s_HolePunching_listen: error\n");
    if (bind(s_HolePunching_listen, (SOCKADDR*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
        printf("bind() failed: %d\n", WSAGetLastError());
        closesocket(s_HolePunching_listen);
        WSACleanup();
        return 1;
    }

    retVal = ioctlsocket(s_HolePunching_listen, FIONBIO, (unsigned long*)&u1);
    retVal = listen(s_HolePunching_listen, 1);
    if (retVal == SOCKET_ERROR) {
        printf("listen failed!\n");
        WSACleanup();
        return 1;
    }

    bool c= false, a = false;
    while (c == false && a == false) {
        if (c == false) {
            retVal = connect(s_HolePunching_connect, (SOCKADDR*)&peerAddr, sizeof(peerAddr));
            if (retVal == SOCKET_ERROR) {
                retVal = WSAGetLastError();
                printf("connect() failed: %d\n", retVal);
            }
            else
                c = true;
        }

        if (a == false) {
            sClient = accept(s_HolePunching_listen, (SOCKADDR*)&clientAddr, &sLen);
            if (sClient == INVALID_SOCKET)
                printf("accept() failed: %d\n", WSAGetLastError());
            else
                a = true;
        }
    }
    printf("TCP之NAT穿透成功...\n");
    if (a == true) {
        ZeroMemory(buf_send, BUF_SIZE);
        strcpy(buf_send, "Hello, this is your peer at ");
        strcat(buf_send, str_local_endpoint);
        strcat(buf_send, "\n");
        if (send(sClient, buf_send, BUF_SIZE, 0) == SOCKET_ERROR)
            printf("send() failed: %d\n", WSAGetLastError());
        ZeroMemory(buf_recv1, BUF_SIZE);
        if (recv(sClient, buf_recv1, BUF_SIZE, 0) == SOCKET_ERROR)
            printf("recv() failed: %d\n", WSAGetLastError());
        else
            printf("%s\n", buf_recv1);
    }

    if (c == true){
        ZeroMemory(buf_send, BUF_SIZE);
        strcpy(buf_send, "Hello, this is your peer at ");
        strcat(buf_send, str_local_endpoint);
        strcat(buf_send, "\n");
        if (send(s_HolePunching_connect, buf_send, BUF_SIZE, 0) == SOCKET_ERROR)
            printf("send() failed: %d\n", WSAGetLastError());
        ZeroMemory(buf_recv1, BUF_SIZE);
        if (recv(s_HolePunching_connect, buf_recv1, BUF_SIZE, 0) == SOCKET_ERROR)
            printf("recv() failed: %d\n", WSAGetLastError());
        else
            printf("%s\n", buf_recv1);
    }

    system("pause");
    closesocket(s);
    WSACleanup();
    return 0;
}
