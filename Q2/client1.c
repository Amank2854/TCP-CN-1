#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{

    int newsocketfd, n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[256];

    // Creating socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        printf("There is an Error opening Socket\n");
        close(socketfd);
        exit(-1);
    }

    int port = 9000;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    server = gethostbyname(hostname);

    if (server == NULL)
    {
        printf("Error, no such host\n");
        close(socketfd);
        exit(-1);
    }

    // Specifying address
    int size_serv = sizeof(server_addr);
    bzero((char *)&server_addr, size_serv);

    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Connecting to the server
    int val = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (val < 0)
    {
        printf("Connection has been Failed\n");
        close(socketfd);
        exit(-1);
    }
    while (1)
    {

        // Reading input from user
        printf("Enter code in format ClientX:Y where X is Client no. and Y is tickets and exit to quit\n");
        bzero(buffer, 256);
        scanf("\n%[^\n]s", buffer);

        if(strcmp(buffer,"exit") == 0)
        {
            close(socketfd);
            break;
        }

        if(strlen(buffer)< 9 || buffer[0] != 'C' || buffer[1] != 'l' || buffer[2]!= 'i' || buffer[3]!='e' || buffer[4]!='n' || buffer[5]!='t' || buffer[7]!=':' || (buffer[6]<'0' || buffer[6]>'9'))
        {
            printf("Invalid Input\n");
            continue;
        }

        int i = 8;
        for(;i<strlen(buffer);i++)
        {
            if(buffer[i] < '0' || buffer[i] > '9')
            {
                i = -1;
                break;
            }
        }

        if(i == -1)
        {
            printf("Invalid Input\n");
            continue;
        }

        // Sending string to the server
        int len = strlen(buffer);
        n = send(socketfd, buffer, len, 0);
        if (n <= 0)
        {
            printf("Error on Writing buffer\n");
            close(socketfd);
            exit(-1);
        }

        bzero(buffer, 256);

        // Receiving output from the server
        n = recv(socketfd, buffer, 256, 0);
        if (n <= 0)
        {
            printf("Error on Reading buffer\n");
            close(socketfd);
            exit(-1);
        }

        // Printing the output and closing the connection
        printf("Server: %s\n", buffer);
    }
}