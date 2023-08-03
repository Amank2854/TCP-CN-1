#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int main()
{
        int port = 9000; // Port Number of server
        pthread_mutex_t mutex;
        int n;
        char buffer[256];
        struct sockaddr_in serv_addr;

        char *username = "server";
        char *password = "server";

        char user[256];
        char pass[256];

        printf("Enter Username: ");
        scanf("%s", user);
        printf("Enter Password: ");
        scanf("%s", pass);

        if (strcmp(user, username) == 0 && strcmp(pass, password) == 0)
        {
                printf("Login Successful\n");
        }
        else
        {
                printf("Login Failed\n");
                exit(0);
        }

        // Creating server socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
                perror("Socket Creation has been Failed\n");
                return 0;
        }
        int len = sizeof(serv_addr);
        bzero((char *)&serv_addr, len);

        // Specifying Server Address
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        // Binding the socket
        len = sizeof(serv_addr);
        int val = bind(sockfd, (struct sockaddr *)&serv_addr, len);
        if (val < 0)
        {
                perror("Binding has been Failed.\n");
                return 0;
        }
        else
        {
                printf("Successfully Binded\n");
        }

        // Listening for connections
        if (listen(sockfd, 10) < 0)
        {
                perror("Error in Listen\n");
                return 0;
        }

        while (1)
        {
                struct sockaddr_in caddr;

                printf("Waiting for incoming connections...\n");

                int clen = sizeof(caddr);
                // Listening incomming connections
                int clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen);

                if (clientfd < 0)
                {
                        perror("Error on Accept.\n");
                        continue;
                }

                pid_t pid = fork();
                if (pid == -1)
                {
                        perror("Failed to fork");
                        close(clientfd);
                        continue;
                }
                else if (pid == 0)
                {
                        while (1)
                        {

                                pthread_mutex_lock(&mutex);

                                bzero(buffer, 256);

                                // Receiving request from client
                                n = recv(clientfd, buffer, 256, 0);
                                if (n < 0)
                                {
                                        perror("Error on Receiving Buffer.\n");
                                        continue;
                                }
                                // This is the child process
                                int recordsfd = open("server_records.txt", O_RDONLY | O_CREAT, 0666);
                                if (recordsfd == -1)
                                {
                                        perror("Failed to open file");
                                        exit(0);
                                }
                                // Seek to the end of the file
                                off_t pos = lseek(recordsfd, 0, SEEK_END) - 1;
                                char ch;
                                while (pos > 0)
                                {
                                        lseek(recordsfd, pos, SEEK_SET);
                                        int readval = read(recordsfd, &ch, 1);
                                        if (readval != 1)
                                        {
                                                perror("Failed to read the file");
                                                close(recordsfd);
                                                exit(0);
                                        }
                                        int flag = 0;
                                        if (ch == 'N')
                                        {
                                                flag = 1;
                                        }
                                        if (flag == 1)
                                        {
                                                break;
                                        }
                                        pos--;
                                }

                                // Read the last line of the file
                                char line[1024] = {'\0'};
                                int preadval = pread(recordsfd, line, 1024, pos);
                                if (preadval > 0)
                                {
                                        // Do Nothing
                                }
                                else
                                {
                                        printf("File is empty\n");
                                }

                                // Close the file
                                close(recordsfd);

                                int tickets_cnt = 0;
                                char *position = strstr(line, ":");
                                sscanf(position + 1, "%d", &tickets_cnt);


                                int client_tickets_cnt = 0;
                                char *pos_1 = strstr(buffer, ":");
                                char *str = strstr(buffer, "t");
                                int clientid = str[1] - '0';
                                sscanf(pos_1 + 1, "%d", &client_tickets_cnt);

                                if (client_tickets_cnt > tickets_cnt)
                                {
                                        char *str = "Sorry, not enough tickets available for booking";
                                        strcpy(buffer, str);
                                }
                                else
                                {

                                        tickets_cnt -= client_tickets_cnt;
                                        char *str = "Successfully booked Tickets for Client.";
                                        strcpy(buffer, str);
                                        int writefd = open("server_records.txt", O_WRONLY | O_APPEND);
                                        if (writefd == -1)
                                        {
                                                perror("Failed to open the file");
                                                exit(0);
                                        }

                                        char strbuf1[2048];
                                        char strbuf2[1024];
                                        char *client_write_Val = "\nClient ";

                                        sprintf(strbuf1, "%s%d Booked %d Tickets", client_write_Val, clientid, client_tickets_cnt);

                                        client_write_Val = strbuf1;
                                        int len1 = strlen(client_write_Val);
                                        write(writefd, client_write_Val, len1);

                                        char *newline = "\nNumber of Tickets Left:";
                                        sprintf(strbuf2, "%s%d", newline, tickets_cnt);
                                        newline = strbuf2;
                                        int lgt = strlen(newline);
                                        write(writefd, newline, lgt);

                                        close(writefd);
                                }

                                // Sending the ouput to client
                                int len3 = strlen(buffer);
                                n = send(clientfd, buffer, len3, 0);
                                if (n < 0)
                                {
                                        perror("Error on Sending.\n");
                                        continue;
                                }

                                bzero(buffer, 256);
                                pthread_mutex_unlock(&mutex);

                                // Closing client connection
                                // close(clientfd);
                                // exit(0);
                        }
                }
        }
        close(sockfd);
}
