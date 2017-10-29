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

int main(){
    WSADATA wsd;
    SOCKET sServer; //server socket
    SOCKET sClient; //client socket
    SOCKADDR_IN addrServ;   //IPv4 server address
    SOCKADDR_IN addrClient; //IPv4 server address
    char buf[BUF_SIZE]; //receive data buffer
    int retVal; //return value

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

    //accept IPv4 client request
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
