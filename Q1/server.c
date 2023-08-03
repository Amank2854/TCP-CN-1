#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Function to reverse a string
void reverse_string(char* str)
{

    for (int i=0; i<strlen(str)/2; i++)
    {
        // Swapping the characters
        char temp = str[i];
        str[i] = str[strlen(str)-i-1];
        str[strlen(str)-i-1] = temp;
    }
}

int main()
{

    int port = 8881;

    int sockfd, n;
    char buffer[256];
    struct sockaddr_in serv_addr;

    printf("Starting Server Socket\n");

    // Creating server socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket Creation Failed\n");
        return 0;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Specifying Server Address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Binding the socket 
    int vv = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (vv < 0){
        perror("Binding Failed.\n");
        return 0;
    }

    // Listening for connections
    int val = listen(sockfd, 10);
    if (val < 0){
        perror("Error in Listen\n");
        return 0;
    }

    printf("Listening...\n");


    while(1)
    {
        struct sockaddr_in caddr;
        
        printf("Waiting for incoming connections...\n");

        int clen = sizeof(caddr);
        // Listening incomming connections
        int clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);

        if (clientfd < 0)
        {
            perror("Error on Accept.\n");
            continue;
        }

        bzero(buffer, 256);

        // Receiving request from client
        if (recv(clientfd, buffer, 256, 0)< 0)
        {
            perror("Error on Receiving buffer.\n");
            continue;
        }

        printf("Client: %s\n", buffer);

        // Reversing the string
        reverse_string(buffer);

        // Sending the ouput to client
        if (send(clientfd, buffer, strlen(buffer), 0)< 0)
        {
            perror("Error on Sending buffer.\n");
            continue;
        }

        bzero(buffer, 256);

        // Closing client connection
        close(clientfd);
    }
    close(sockfd);
}