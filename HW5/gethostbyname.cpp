#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#pragma comment(lib, ""ws2_32.lib)

int main(){
    WSADATA wsd;
    struct hostent *he;
    char hostname[128];

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
        printf("WSAStartup Failed!\n");
        return 1;
    }

    gethostname(hostname, sizeof hostname);
    printf("The hostname: %s\n", hostname);

    if ((he = gethostbyname(hostname)) == NULL){
        DWORD dwError = WSAGetLastError();
        if (dwError != 0){
            if (dwError == WSAHOST_NOT_FOUND){
                printf("Host Not Found!\n");
                return 1;
            }
            else if (dwError == WSANO_DATA){
                printf("No Data Record Found!\n");
                return 1;
            }
            else {
                printf("Function Failed With Error: %1d\n", dwError);
                return 1;
            }
        }
    }

    printf("The Hostname: %s\n", he->h_name);
    printf("IP Address:\n");

    if (he->h_addrtype == AF_INET){
        struct in_addr **addr_list;
        addr_list = (struct in_addr **)he->h_addr_list;
        for (int i = 0 ; addr_list[i] != NULL ; i++)
            printf("\tIP_%d: %s\n", i, inet_ntoa(*addr_list[i]));
    }

    for (int i = 0 ; he->h_aliases[i] != NULL ; i++)
        printf("\tAlternate name #%d: %s\n", i, *he->h_aliases[i]);

    system("pause");
    return 0;
}
