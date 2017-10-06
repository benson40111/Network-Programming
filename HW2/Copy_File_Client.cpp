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
#define BUF_SIZE 1024
#include<iostream>
#include<fstream>
using namespace std;

int main(){
    WSADATA wsd;
    SOCKET sHost;   //server socket
    struct addrinfo hints, *res;
    char buf[BUF_SIZE]; //receive data buffer
    int retVal; //return value
    ifstream input;
    char ip_file[] = "01.jpg";

    //initialization DLL
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Falied!\n");
        return -1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("127.0.0.1", "4999", &hints, &res);

    //Build Internet Socket
    sHost = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (INVALID_SOCKET == sHost){
        printf("Socket Failed!\n");
        WSACleanup();   //if DLL is not using, need to release resource
        return -1;
    }

    //Connect to server
    retVal = connect(sHost, res -> ai_addr, res -> ai_addrlen);
    if (SOCKET_ERROR == retVal){
        printf("Connect Failed!\n");
        closesocket(sHost);  //No using socket need to close too
        WSACleanup();
        return -1;
    }

    //Send data to server
    input.open(ip_file, ios::in|ios::binary);
    if (input.fail()){
        cout << "Input File Opening Failed!\n" << endl;
    }

    while(!input.eof()){
        ZeroMemory(buf, BUF_SIZE);
        input.read((char*)buf, sizeof(char)*BUF_SIZE);
        retVal = send(sHost, buf, BUF_SIZE, 0);

        if (SOCKET_ERROR == retVal){
            printf("Send Failed!\n");
            WSACleanup();
            return -1;
        }
    }

    ZeroMemory(buf, BUF_SIZE);
    strcpy(buf , "EOF");
    retVal = send(sHost, buf, strlen(buf), 0);

    printf("Sent successful!\n");
    system("pause");

    input.close();
    closesocket(sHost);
    WSACleanup();
    return 0;
}
