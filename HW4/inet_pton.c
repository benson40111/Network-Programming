/*-----Important!!!-----*/
/*if error occur, using cmd and enter command below:
    CD C:\Program Files (X86)\CodeBlocks\MinGW\bin
    gcc your_file_path -lws2_32
    You will see a output file which name is a.exe
*/

#define _WIN32_WINNT 0x501
#include<stdio.h>
#include<Winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main(){
    struct sockaddr_in sa_ipv4;
    char str_ipv4[INET_ADDRSTRLEN];

    inet_pton(AF_INET, "192.0.2.33", &(sa_ipv4.sin_addr));

    inet_ntop(AF_INET, &(sa_ipv4.sin_addr), str_ipv4, INET_ADDRSTRLEN);

    printf("%s\n", str_ipv4);

    struct sockaddr_in6 sa_ipv6;
    char str_ipv6[INET6_ADDRSTRLEN];

    inet_pton(AF_INET6, "2001:db8:8714:3a90::12", &(sa_ipv6.sin6_addr));

    inet_ntop(AF_INET6, &(sa_ipv6.sin6_addr), str_ipv6, INET6_ADDRSTRLEN);

    printf("%s\n", str_ipv6);
    system("pause");
    return 0;
}
