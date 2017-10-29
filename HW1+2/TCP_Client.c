/*-----Important!!!-----*/
/*if error occur, using cmd and enter command below:
    CD C:\Program Files (X86)\CodeBlocks\MinGW\bin
    gcc your_file_path -lws2_32
    You will see a output file which name is a.exe
*/

#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib");

#define BUF_SIZE 64
#define PORT 4999
#define IP "127.0.0.1"  //127.0.0.1 is localhost

int main(){
    WSADATA wsd;
    SOCKET sHost;   //server socket
    SOCKADDR_IN servAddr;   //IPv4 server address
    char buf[BUF_SIZE]; //receive data buffer
    int retVal; //return value

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Falied!\n");
        return -1;
    }

    //build Internet Socket
    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sHost){
        printf("Socket Failed!\n");
        WSACleanup();   //if DLL is not using, need to release resource
        return -1;
    }

    //Setting server address
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);
    int nServAddlen = sizeof(servAddr);

    //connect to server
    retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
    if (SOCKET_ERROR == retVal){
        printf("connect failed!\n");
        closesocket(sHost);  //No using socket need to close too
        WSACleanup();
        return -1;
    }

    //send data to server
    ZeroMemory(buf, BUF_SIZE);
    strcpy(buf, "test!\n");
    retVal = send(sHost, buf, strlen(buf), 0);
    if (SOCKET_ERROR == retVal){
        printf("Send Failed!\n");
        WSACleanup();
        return -1;
    }

    printf("Sent successful!\n");
    system("pause");

    closesocket(sHost);
    WSACleanup();
    return 0;
}
