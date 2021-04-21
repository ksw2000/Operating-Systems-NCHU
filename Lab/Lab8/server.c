#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define PortNumber 1450

int main(){
    struct sockaddr_in address;
    int sock, byte_recv, address_length;
    char buffer[1024];
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        fprintf(stderr, "Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PortNumber);
    address.sin_addr.s_addr = INADDR_ANY; // Accept all ip
    if(bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1){
        printf("error binding\n");
        close(sock);
    }

    char response_msg[] = "Welcome";
    for(;;){
        byte_recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, &address_length);
        if(byte_recv < 0){
            printf("Error receiving packet\n");
        }
        printf("data: %s\n", buffer);
        int byte_sent = sendto(sock, response_msg, sizeof(response_msg), 0, (struct sockaddr *)&address, address_length);
        if(byte_sent < 0){
            printf("Error sending packet\n");
        }
    }
}
