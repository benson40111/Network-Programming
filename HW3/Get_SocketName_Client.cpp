/*-----Important!!!-----*/
/*if error occur, using cmd and enter command below:
    CD C:\Program Files (X86)\CodeBlocks\MinGW\bin
    gcc your_file_path -lws2_32
    You will see a output file which name is a.exe
*/

#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 4999
#define IP "127.0.0.1"
#define BUF_SIZE 64

int main(){
    WSADATA wsd;
    SOCKET sHost;
    SOCKADDR_IN servAddr;
    char buf[BUF_SIZE];
    int retVal;

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return -1;
    }

    //build Internet Socket
    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sHost){
        printf("Socket Failed!\n");
        WSACleanup();   //if DLL is not using, need to release resource
        return -1;
    }

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

    if (SOCKET_ERROR == retVal){
        printf("Connect Failed!\n");
        closesocket(sHost);
        WSACleanup();
        return -1;
    }

    system("pause");
    closesocket(sHost);
    WSACleanup();
    return 0;
}
