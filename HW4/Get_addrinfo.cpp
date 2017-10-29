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

int main(){
    WSADATA wsaData;

    int i = 1;

    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    struct sockaddr_in *sockaddr_ipv4;

    int iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (iResult != 0){
        printf("WSAStartup Failed: %d\n", iResult);
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int rv = getaddrinfo(NULL, "5000", &hints, &result);
    if (rv != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        WSACleanup();
        return -1;
    }

    for (ptr = result ; ptr != NULL ; ptr = ptr -> ai_next){
        printf("getaddressinfo response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr -> ai_flags);
        printf("\tFamily: ");
        switch(ptr -> ai_family){
            case AF_UNSPEC:
                printf("Unspecified!\n");
                break;
            case AF_INET:
                printf("AF_INET (IPv4)\n");
                sockaddr_ipv4 = (struct sockaddr_in)ptr -> ai_addr;
                printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4 -> sin_addr));
                break;
            case AF_INET6:
                printf("AF_INET6 (IPv6)\n");
                struct sockaddr_in6 *sockaddr_ipv6;
                sockaddr_ipv6 = (struct sockaddr_in6*)ptr -> ai_addr;
                char ipstringbuffer[46];
                printf("\tIPv6 address %s\n", inet_ntop(AF_INET6, &sockaddr_ipv6 -> sin6_addr, ipstringbuffer, 46);
                break;
            default:
                printf("Other %1d\n", ptr -> ai_family);
                break;
            }

        printf("\tSocket Type: ");
        switch (ptr -> ai_socktype){
            case 0:
                printf("Unspecified!\n");
                break;
            case SOCK_STREAM:
                printf("SOCK_STREAM (stream)\n");
                break;
            case SOCK_DGRAM:
                printf("SOCK_DGRAM (datagram)\n");
                break;
            case SOCK_RAW:
                printf("SOCK_RAW (raw)\n");
                break;
            case SOCK_RDM:
                printf("SOCK_RDM (reliable message datagram)\n");
                break;
            case SOCK_SEQPACKET:
                printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
                break;
            default:
                printf("Other %1d\n", ptr -> ai_socktype);
                break;
        }

        printf("\tProtocol: ");
        switch (ptr -> ai_protocol){
            case 0:
                printf("Unspecified!\n");
                break;
            case IPPROTO_TCP:
                printf("IPPROTO_TCP (TCP)\n");
                break;
            case IPPROTO_UDP:
                printf("IPPROTO_UDP (UDP)\n");
                break;
            default:
                printf("Other %1d\n", ptr -> ai_protocol);
                break;
        }
        printf("\tLength of this sockaddr: %d\n", ptr -> ai_addrlen);
        printf("\tCanonical name: %s\n", ptr -> ai_canonname);
    }

    freeaddrinfo(result);
    freeaddrinfo(ptr);
    WSACleanup();
    system("pause");
    return 0;
}
