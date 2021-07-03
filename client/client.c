// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread,chunk;
    chunk = 100000;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    while(1){
    	printf("client>");
        int flag = 3;
    	char inp[256] = {0};
    	int count= 0 ,filesize = -1,dest,bwritten,bs;
    	fgets(inp,256,stdin);

    	char command[256][256];
    	char *tok = strtok(inp," \t\n");
    	while(tok  != NULL){
    		strcpy(command[count],tok);
			tok = strtok(NULL," \t\n"); 
    		count++;
    	}
        //printf("count : %d\n",count );

    	if(strcmp(command[0],"exit") == 0){
            //flag  = 0;
            //send(sock,&flag,sizeof(int),0);
    		close(sock);
    		exit(0);
    	}

    	else if(strcmp(command[0],"get") == 0){
    		for(int i = 1;i<count;i++){
                flag = 1;
    			char filename[300] = {0};
    			strcpy(filename,command[i]);
             //   send(sock,&flag,sizeof(int),0);
    			if(send(sock,filename,strlen(filename),0) < 0)
    			{
    				printf("Error sending filename\n");
    				continue;
    			}
    			valread = read(sock,&filesize,sizeof(int));
    			if(filesize <0){
    				printf("Given file doesn't exists or not has enough permissions\n");
    			}
    			else{
    			char buffer[chunk];
    			dest = open(filename,O_RDWR | O_CREAT | O_TRUNC,0644);
                if(dest < 0){
                    printf("Error creating file\n");
                }
    			bwritten = 0;
                send(sock,"start",strlen("start"),0);
	    			while(bwritten < filesize){
	    				bzero(buffer,chunk);
	    				valread = read(sock,buffer,chunk);
	    				if(valread < 0){
	    					printf("Read Error\n");
	    					break;
	    				}
	    				buffer[valread] = '\0';
	    				write(dest,buffer,valread);
	    				bwritten += valread;
	    				printf("\r %.2f %% downloaded\r",(float)(((float)bwritten)/(float)filesize)*100);
	    			}
	    			printf("\rDownload Complete\n");
    			}
    		}
    	}
    	else{
            flag = 2;
           // send(sock,&flag,sizeof(int),0);
    		printf("INVALID COMMAND\n");
    	}
        bzero(inp,256);
        bzero(command,256);
    }
   /* send(sock , hello , strlen(hello) , 0 );  // send the message.
    printf("Hello message sent\n");
    valread = read( sock , buffer, chunk);  // receive message back from server, into the buffer
    printf("%s\n",buffer);*/
    return 0;
}
