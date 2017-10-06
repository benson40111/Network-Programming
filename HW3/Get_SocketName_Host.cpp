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
#define BUF_SIZE 64

int main(){
    WSADATA wsd;
    SOCKET sServer;
    SOCKET sClient;
    SOCKADDR_IN addrServ;
    SOCKADDR_IN addrClient;
    char buf[BUF_SIZE];
    int retVal;

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return -1;
    }

    //build Internet Socket
    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sServer){
        printf("Socket Failed!\n");
        WSACleanup();   //if DLL is not using, need to release resource
        return -1;
    }

    //Setting server address
    addrServ.sin_family = AF_INET;
    addrServ.sin_addr.s_addr = INADDR_ANY;  //auto fill in IP address
    addrServ.sin_port = htons(PORT);

    //bind socket
    retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
    if (SOCKET_ERROR == retVal){
        printf("Bind Failed!\n");
        closesocket(sServer);   //No using socket need to close too
        WSACleanup();
        return -1;
    }

    //start listening
    retVal = listen(sServer, 1);
    if (SOCKET_ERROR == retVal){
        printf("Listen Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    struct sockaddr_in local;
    int saSize = sizeof(local);
    memset(&local, 0, sizeof(local));
    getsockname(sServer, (struct sockaddr *)&local, &saSize);
    printf("The address of the listening socket: %s:%d\n", inet_ntoa(local.sin_addr), ntohs(local.sin_port));

    int addrClientlen = sizeof(addrClient);
    sClient = accept(sServer, (SOCKADDR FAR*)&addrClient, &addrClientlen);
    if (INVALID_SOCKET == sClient){
        printf("Accept Failed!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    memset(&local, 0, sizeof(local));
    getsockname(sClient, (struct sockaddr *)&local, &saSize);
    printf("The address of the accepted socket: %s:%d\n", inet_ntoa(local.sin_addr), ntohs(local.sin_port));

    system("pause");
    return 0;
}
