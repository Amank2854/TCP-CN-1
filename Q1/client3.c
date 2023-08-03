#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int main()
{

    // Initializing variables
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    // Creating socket 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error opening Socket\n");
        close(sockfd);
        exit(-1);
    }

    int port = 8881;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    server = gethostbyname(hostname);

    if (server == NULL)
    {
        printf("Error, no such host\n");
        close(sockfd);
        exit(-1);
    }

    // Specifying address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    // Connecting to the server
    int val = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if( val < 0 )
    {
        printf("Connection Failed\n");
        close(sockfd);
        exit(-1);
    }

    printf("Entered CN LAB4\n");
    bzero(buffer, 256);
    strcpy(buffer, "CN LAB4");

    // Sending string to the server
    if (send(sockfd, buffer, strlen(buffer), 0)<=0)
    {
        printf("Error on Writing\n");
        close(sockfd);
        exit(-1);
    }
    bzero(buffer, 256);

    if (recv(sockfd, buffer, 256, 0)<=0)
    {
        printf("Error on Reading\n");
        close(sockfd);
        exit(-1);
    }

    // Printing the output and closing the connection
    printf("Server: %s\n", buffer);
    close(sockfd);
}