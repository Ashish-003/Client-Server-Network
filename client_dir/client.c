#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define PORT 8085
#define CHUNK_SIZE 1024
int main()
{
    int sock = 0, valRead;

    struct sockaddr_in servAddr;
    char *welcomeMessage = "Thankyou for accepting the connection";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Connection error\n");
        return -1;
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0)
    // {
    //     printf("Invalid address\n");
    //     return -1;
    // }

    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }
    printf("Connection Succeeded\n");
    char *commmands[256];

    while (1)
    {

        printf("\033[1;32mclient>\033[0m");
        char str[256] = {0};
        fgets(str, 256, stdin);

        char *tok = strtok(str, "\n\t ");
        int numCommands = 0;
        while (tok != NULL)
        {
            commmands[numCommands] = (char *)malloc(sizeof(char) * 256);
            strcpy(commmands[numCommands], tok);
            tok = strtok(NULL, "\n\t ");
            numCommands++;
        }
        // commmands[numCommands] = NULL;
        // for (int i = 0; i < numCommands; i++)
        //     printf("%s\n", commmands[i]);
        if (strcmp(commmands[0], "exit") == 0)
        
            break;

        else if (strcmp(commmands[0], "get") == 0)
        {
            for (int i = 1; i < numCommands; i++)
            {
                char filename[256];
                strcpy(filename, commmands[i]);
                printf("filename: %s\n", filename);
                if (send(sock, filename, strlen(filename), 0) < 0)
                {
                    printf("\033[1;31mReceiving %s failed\033[0m\n", filename);
                    continue;
                }
                char buffer[CHUNK_SIZE] = {0};
                if ((valRead = recv(sock, buffer, CHUNK_SIZE, 0)) < 0)
                {
                    printf("Receiving failed\n");
                    continue;
                }
                if (strcmp(buffer, "Y") == 0)
                {
                    printf("File %s found on the server\n", filename);
                }
                if (strcmp(buffer, "N") == 0)
                {
                    printf("\033[1;31mFile %s not found on the server.\033[0m\n", filename);
                    continue;
                }

                FILE *file = fopen(filename, "ab");
                if (file == NULL)
                {
                    printf("Error creating the file.");
                    // send(sock, "N", strlen("N"), 0);
                    return 1;
                    // continue;
                }
                int bytesRec;
                while (1)
                {

                    bytesRec = recv(sock, buffer, CHUNK_SIZE, 0);
                    printf("%s\n", buffer);
                    printf("Bytes received %d\n", bytesRec);
                    fwrite(buffer, 1, bytesRec, file);
                    if (bytesRec <= 0)
                    {
                        printf("Read Error\n");
                        break;
                    }
                    if (bytesRec < CHUNK_SIZE)
                    {
                        printf("File transfer finished\n");
                        fclose(file);
                        break;
                    }
                }
            }
        }
        else printf("\033[1;31mCommand %s not recognized\n",commmands[0]);
    }
    close(sock);
    return 0;
}