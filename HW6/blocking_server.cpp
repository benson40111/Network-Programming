#include<iostream>
#include<winsock2.h>
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define SERVER_EXIT_OK  0
#define SERVER_DLL_ERROR    1
#define SERVER_API_ERROR    2
#define SERVERPORT  5555
#define MAX_NUM_BUF 64

char bufRecv[MAX_NUM_BUF];
char bufSend[MAX_NUM_BUF];
SOCKET sServer;
SOCKET sClient;
BOOL bConning;

void InitMember(void);
int ExitClient(int nExit);
BOOL RecvLine(SOCKET s, char* buf);
BOOL SendLine(SOCKET s, char* buf);
int HandleSocketError(char* str);
void ShowSocketMsg(char* str);

int main(){
    InitMember();

    WSADATA wsaData;
    int retVal;
    retVal = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (0 != retVal){
        ShowSocketMsg("Can not find a usable Windows Sockets dll!");
        return SERVER_DLL_ERROR;
    }

    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sServer){
        return HandleSocketError("Failed socket()!");
    }

    SOCKADDR_IN addrServ;
    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(SERVERPORT);
    addrServ.sin_addr.s_addr = INADDR_ANY;

    retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
    if (SOCKET_ERROR == retVal){
        closesocket(sServer);
        return HandleSocketError("Failed bind()!");
    }

    retVal = listen(sServer, 1);
    if (SOCKET_ERROR == retVal){
        closesocket(sServer);
        return HandleSocketError("Failed listen()!");
    }

    cout << "Server succeeded!" << endl;
    cout << "Waiting for new clients..." << endl;

    sockaddr_in addrClient;
    int addrClientlen = sizeof(addrClient);
    sClient = accept(sServer, (sockaddr *FAR)&addrClient, &addrClientlen);
    if (INVALID_SOCKET == sClient){
        closesocket(sServer);
        return HandleSocketError("Failed accept()!");

    }
    else{
        bConning = TRUE;
    }

    char *pClientIP = inet_ntoa(addrClient.sin_addr);
    u_short clientPort = ntohs(addrClient.sin_port);
    cout << "Accept a client." << endl;
    cout << "IP: " << pClientIP << endl;
    cout << "Port: " << clientPort << endl;

    if (!RecvLine(sClient, bufRecv)){
        return ExitClient(SERVER_API_ERROR);
    }

    cout << bufRecv << endl;

    strcpy(bufSend, "Hello, Client!\n");
    if (!SendLine(sClient, bufSend)){
        return ExitClient(SERVER_API_ERROR);
    }

    cout << "Server exiting..." << endl;
    system("pause");

    return ExitClient(SERVER_EXIT_OK);
}

void InitMember(void){
    memset(bufRecv, 0, MAX_NUM_BUF);
    memset(bufSend, 0, MAX_NUM_BUF);

    sServer = INVALID_SOCKET;
    sClient = INVALID_SOCKET;

    bConning = FALSE;
}

int ExitClient(int nExit){
    closesocket(sServer);
    closesocket(sClient);
    WSACleanup();
    return nExit;

}

BOOL RecvLine(SOCKET s, char* buf){
    BOOL retVal = TRUE;
    BOOL bLineEnd = FALSE;
    int nReadLen = 0;
    int nDataLen = 0;
    while (!bLineEnd && bConning){
        nReadLen = recv(s, buf + nDataLen, 1, 0);

        if (SOCKET_ERROR == nReadLen){
            int nErrCode = WSAGetLastError();
            if (WSAENOTCONN == nErrCode){
                ShowSocketMsg("The socket is not connected!");
            }
            else if (WSAESHUTDOWN == nErrCode){
                ShowSocketMsg("The socket has been shut down!");
            }
            else if (WSAETIMEDOUT == nErrCode){
                ShowSocketMsg("The connection has been dropped!");
            }
            else if (WSAECONNRESET == nErrCode){
                ShowSocketMsg("The virtual circuit was reset by the remote side!");
            }
            else{}

            retVal = FALSE;
            break;
        }

        if (0 == nReadLen){
            retVal = FALSE;
            break;
        }

        if ('\n' == *(buf + nDataLen)){
            bLineEnd = TRUE;
        }
        else{
            nDataLen += nReadLen;
        }
    }
    return retVal;

}

BOOL SendLine(SOCKET s, char* str){
    int retVal;

    if (SOCKET_ERROR == retVal){
        int nErrCode = WSAGetLastError();
        if (WSAENOTCONN == nErrCode){
            ShowSocketMsg("The socket is not connected!");
        }
        else if (WSAESHUTDOWN == nErrCode){
            ShowSocketMsg("The socket has been shut down!");
        }
        else if (WSAETIMEDOUT == nErrCode){
            ShowSocketMsg("The connection has been dropped!");
        }
        else{}

        return FALSE;
    }
    return TRUE;
}

int HandleSocketError(char *str){
    ShowSocketMsg(str);
    WSACleanup();
    return SERVER_API_ERROR;
}

void ShowSocketMsg(char* str){
    MessageBox(NULL, str, "SERVER ERROR", MB_OK);
}






