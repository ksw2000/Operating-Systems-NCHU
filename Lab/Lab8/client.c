#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define Server_PortNumber 1450
#define Server_Address "127.0.0.1"
int main(){
    struct sockaddr_in address;
    int sock, byte_sent;
    char buffer[1024];

    // SOCK_DGRAM = UDP, SOCK_STREAM = TCP
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        fprintf(stderr, "Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;   // AF_INET: Address Family is IPv4
    address.sin_port = htons(Server_PortNumber);
    address.sin_addr.s_addr = inet_addr(Server_Address);
    int address_length = sizeof(address);

    for(;;){
        printf("Please enter a string to send to server:");
        scanf("%s", buffer);
        byte_sent = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&address, address_length);
        if(byte_sent < 0){
            printf("Error sending packet\n");
        }

        recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&address, &address_length);
        printf("Receiving from server: %s\n", buffer);
    }

    return 0;
}
