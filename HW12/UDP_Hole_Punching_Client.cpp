#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#define BUF_SIZE 64
#define PORT 5000
#define IP "127.0.0.1"

#pragma comment(lib, "ws2_32.lib")
bool ACK = false, SYN_Message_Rcv = false;
DWORD __stdcall RcvThread(void *pParam);

int main(){
    WSADATA wsd;
    SOCKADDR_IN servAddr, tmpAddr, peerAddr;
    char buf_SYN[BUF_SIZE], buf_rcv1[BUF_SIZE], buf_rcv2[BUF_SIZE];
    int sLen = sizeof(SOCKADDR_IN);
    int peerPort;
    double timeout = 1;

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
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);

    struct hostent* hostEntry;
    char hostname[BUF_SIZE];
    gethostname(hostname, BUF_SIZE);
    hostEntry = gethostbyname(hostname);
    struct in_addr ** addr_list;

    ZeroMemory(buf_SYN, BUF_SIZE);
    strcpy(buf_SYN, "(Registration message)");
    if (sendto(s, buf_SYN, BUF_SIZE, 0, (SOCKADDR*)&servAddr, sLen) == SOCKET_ERROR)
        printf("sendto() failed: %d\n", WSAGetLastError());
    printf("已發送註冊訊息到 Rendezvous Server.\n");

    struct sockaddr_in local;
    memset(&local, 0, sLen);
    getsockname(s, (struct sockaddr*)&local, &sLen);
    int local_port = ntohs(local.sin_port);

    char str_local_endpoint[BUF_SIZE];
    memset(&str_local_endpoint, 0, BUF_SIZE);
    strcpy(str_local_endpoint, inet_ntoa(*addr_list[0]));
    strcat(str_local_endpoint, ":");
    memset(&buf_rcv1, 0, BUF_SIZE);
    sprintf(buf_rcv1, "%d", local_port);
    strcat(str_local_endpoint, buf_rcv1);

    printf("接收從 Rendezvous Server送來之 Peer 的 endpoint(IP:Port), 並將之寫入Socket address 'peerAddr'");
    ZeroMemory(buf_rcv1, BUF_SIZE);
    if (recvfrom(s, buf_rcv1, BUF_SIZE, 0, (SOCKADDR*)&tmpAddr, &sLen) == SOCKET_ERROR)
        printf("recvfrom() failed: %d\n", WSAGetLastError());
    memset(&buf_rcv2, 0, BUF_SIZE);
    strcpy(buf_rcv2, buf_rcv1);

    int ip_len = strlen(buf_rcv1);
    memset(&buf_rcv1, 0, BUF_SIZE);
    strcpy(buf_rcv1, buf_rcv2 + ip_len + 1);
    sscanf(buf_rcv1, "%d", &peerPort);
    peerAddr.sin_port = htons(peerPort);
    printf("\t--> %s\n", buf_rcv2);

    HANDLE hThreadRcv;
    unsigned long ulThreadId;
    hThreadRcv = CreateThread(NULL, 0, RcvThread, NULL, 0, &ulThreadId);

    ZeroMemory(buf_SYN, BUF_SIZE);
    strcpy(buf_SYN, str_local_endpoint);
    int retVal = sendto(s, buf_SYN, BUF_SIZE, 0, (SOCKADDR*)&peerAdd, sLen);
    while (retVal == SOCKET_ERROR) {
        printf("sendto() failed: %d\n", WSAGetLastError());
        retVal = sendto(s, buf_SYN, BUF_SIZE, 0, (SOCKADDR*)&peerAddr, sLen);
    }
    printf("已送出傳送第一個打洞的訊息至Peer\n");

    unsigned long ul = 1;

    char buf_ACK[BUF_SIZE];
    ZeroMemory(buf_ACK, BUF_SIZE);
    strcpy(buf_ACK, "~ACK~");
    double duration;
    clock_t start, finish;
    start = clock();
    bool ACK_Sent = false;
    while (ACK == false || SYN_Message_Rcv == false) {
        if (SYN_Message_Rcv == true && ACK_Sent == false) {
            retVal = sendto(s, buf_ACK, BUF_SIZE, 0, (SOCKADDR*)&peerAddr, sLen);
            if (retVal == SOCKET_ERROR)
                printf("(ACK) sendto() failed: %d\n", WSAGetLastError());
            else
                ACK_Sent = true;
        }

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        if (ACK == false && duration > timeout) {
            retVal = sendto(s, buf_SYN, BUF_SIZE, 0, (SOCKADDR*)&peerAddr, sLen);
            if (retVal == SOCKET_ERROR)
                printf("(SYN) sendto() failed: %d\n", WSAGetLastError());
            start = clock();
        }
    }

    printf("P2P hole punching succeeds.\n");
    system("pause");
    closesocket(s);
    WSACleanup();
    return 0;
}

DWORD __stdcall RcvThread(void *pParam) {
    char buf_rcv[BUF_SIZE];
    SOCKADDR_IN tmpAddr;
    int sLen = sizeof(SOCKADDR_IN);
    ZeroMemory(buf_rcv, BUF_SIZE);
    while (ACK == false || SYN_Message_Rcv == false) {
        if (recvfrom(s, buf_rcv, BUF_SIZE, 0, (SOCKADDR*)&tmpAddr, &sLen) == SOCKET_ERROR)
            printf("recvfrom() failed: %d\n", WSAGetLastError());
        else {
            if (strncmp(buf_rcv, "~ACK~", 5) == 0) {
                ACK = true;
                printf("成功穿透Peer的NAT (已收到Peer送來的ACK)\n");
            }
            else {
                if (SYN_Message_Rcv == false) {
                    SYN_Message_Rcv = true;
                    printf("本機的NAT被穿透成功(Peer: %s)\n", buf_rcv);
                }
            }
        }
    }
    return 0;
}








