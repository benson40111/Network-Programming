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

int main(){
    WSADATA wsd;
    SOCKET sServer;
    SOCKET sClient;
    struct addrinfo hints, *res;    //IPv4 or IPv6 server address
    struct sockaddr_storage addrClient; //IPv4 or IPv5 client address
    char buf[BUF_SIZE]; //receive data buffer
    int retVal;

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  //AF_UNSPEC couldn't run on PC
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, "4999", &hints, &res);

    //Build Internet Socket
    sServer = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (INVALID_SOCKET == sServer){
        printf("Socket Failed!\n");
        WSACleanup();
        return -1;
    }

    //Bind Socket
    retVal = bind(sServer, res -> ai_addr, res -> ai_addrlen);
    if (SOCKET_ERROR == retVal){
        printf("Bind Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    //Start Listen
    retVal = listen(sServer, 1);
    if (SOCKET_ERROR == retVal){
        printf("Listen Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    //accept client request
    int addrClientlen = sizeof(addrClient);

    printf("Ready to receive data!\n");
    sClient = accept(sServer, (SOCKADDR FAR*)&addrClient, &addrClientlen);
    if (INVALID_SOCKET == sClient){
        printf("Accept Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    //receive client data
    ZeroMemory(buf, BUF_SIZE);  //set buffer to 0 before receiving
    retVal = recv(sClient, buf, BUF_SIZE, 0);
    if (SOCKET_ERROR == retVal){
        printf("Receive Failed!\n");
        closesocket(sServer);
        closesocket(sClient);
        WSACleanup();
        return -1;
    }

    printf("%s\n", buf);    //print to screen
    system("pause");

    closesocket(sServer);
    closesocket(sClient);
    WSACleanup();
    return 0;
}
