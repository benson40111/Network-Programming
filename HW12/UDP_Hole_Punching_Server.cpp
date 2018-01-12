#include<stdio.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<stdlib.h>
#define BUF_SIZE 64
#define PORT 5000

#pragma comment(lib, "ws2_32.lib")

int mian(){
    WSADATA wsd;
    SOCKET s;
    SOCKADDR_IN servAddr;
    SOCKADDR_IN clientAddr[2];
    char buf[BUF_SIZE];
    char client_ip_str[2][INET6_ADDRSTRLEN];
    int client_port[2];
    char endpoint_str[2][BUF_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        printf("WSAStartup Failed!\n");
        return 1;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
        printf("socket() failed; %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
            printf("bind() failed: %d\n", WSAGetLastError());
            closesocket(s);
            WSACleanup();
            return 1;
    }

    int nClientLen = sizeof(clientAddr[0]);
    ZeroMemory(buf, BUF_SIZE);
    printf("Ready to receive data...\n");
    int i = 0;
    while (i < 2){
        ZeroMemory(buf, BUF_SIZE);
        ZeroMemory(endpoint_str[i], BUF_SIZE);
        if (recvfrom(s, buf, BUF_SIZE, 0, (SOCKADDR*)&clientAddr[i], &nClientLen) == SOCKET_ERROR)
            printf("recvfrom() failed: %d\n", WSAGetLastError());
        else {
            inet_ntop(AF_INET, &clientAddr[i].sin_addr, client_ip_str[i], sizeof client_ip_str[i]);
            client_port[i] = ntohs(clientAddr[i].sin_port);

            strcpy(endpoint_str[i], client_ip_str[i]);
            strcat(endpoint_str[i], ":");
            char p_str[BUF_SIZE];
            sprintf(p_str, "%d", client_port[i]);
            strcat(endpoint_str[i], p_str);
            printf("From client %d: %s\t%s\n", i, endpoint_str[i], buf);
            i++;
        }
    }

    if (sendto(s, endpoint_str[1], BUF_SIZE, 0, (SOCKADDR*)&clientAddr[0], nClientLen) == SOCKET_ERROR)
        printf("sendto() failed: %d\n", WSAGetLastError());
    if (sendto(s, endpoint_str[0], BUF_SIZE, 0, (SOCKADDR*)&clientAddr[1]. nClientLen) == SOCKET_ERROR)
        printf("sendto() failed: %d\n", WSAGetLastError());
    printf("雙方的endpoint資訊已送出...\n");

    system("pause");
    closesocket(s);
    WSACleanup();
    return 0;
}
