#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8085
#define CHUNK_SIZE 1024

int main()
{
    struct sockaddr_in addr;
    int it = 2;
    int addrlen = sizeof(addr);
    char *welcomeMessage = "Connection made by the server. Happy to see you, my client.";

    int serverFD, newSocket, valRead;

    if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Serverside socket creation failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(serverFD, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &it, sizeof(it)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);

    if (bind(serverFD, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    if (listen(serverFD, 1) < 0)
    {
        perror("Listen");
        exit((EXIT_FAILURE));
    }
    printf("Accepting...\n");
    if ((newSocket = accept(serverFD, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        char buffer[1024] = {0};
        valRead = recv(newSocket, buffer, 1024, 0);
        // printf("Client says: get %s\n", buffer);
        printf("Checking avaibility of the file... %s\n", buffer);

        FILE *file = fopen(buffer, "rb");
        if (file == NULL)
        {
            printf("Error opening file %s\n", buffer);
            send(newSocket, "N", strlen("N"), 0);
            continue;
        }
        else
            send(newSocket, "Y", strlen("Y"), 0);

        // strcpy(welcomeMessage,"");
        printf("Preparing to send the file... %s\n", buffer);
        while (1)
        {

            // send the data in packets
            unsigned char buff[CHUNK_SIZE] = {0};
            int nread = fread(buff, 1, CHUNK_SIZE, file);
            if (nread > 0)
            {
                printf("Sending chunk...\n");
                // send(newSocket, "Y", strlen("S"), 0);
                send(newSocket, buff, nread, 0);
            }
            if (nread < CHUNK_SIZE)
            {
                if (feof(file))
                {
                    printf("End of file\n");
                    // send(newSocket, "N", strlen("S"), 0);
                }
                if (ferror(file))
                {
                    // send(newSocket, "E", strlen("E"), 0);
                    printf("Error reading\n");
                }
                break;
            }
        }
        // sleep(2);

        // send(newSocket, welcomeMessage, strlen(welcomeMessage), 0);
        printf("File sent\n");
    }
    close(newSocket);
    close(serverFD);
    return 0;
}