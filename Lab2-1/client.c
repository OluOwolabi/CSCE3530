//Compilation: gcc -o client client.c
//Name: Olu Owolabi
//Course: CSCE 3530
#include <stdio.h>                      
#include <sys/socket.h>         
#include <netinet/in.h>
#include <netdb.h>
#include <error.h>
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
#define MAXBUF 25000
int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[MAXBUF];

	if (argc < 3)
	{
		printf("\nPort number is missing\n");
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error(EXIT_FAILURE, 0, "ERROR opening socket");
	}
	server = gethostbyname(argv[1]); //IP address of server
	if(server == NULL){
		fprintf(stderr,"Error, no such host");
		exit(0);
	}

	//Connecting with the server
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);
	if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
		error(EXIT_FAILURE, 0, "ERROR connecting the server...");

	//Sending the message to the server
	printf("\nURL:");
	bzero(buffer,MAXBUF);
	scanf("%s", buffer);
	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
	{
		error(EXIT_FAILURE, 0, "ERROR writing to socket");
	}
	while(1)
	{
	//Receiving the message from the client
	bzero(buffer,MAXBUF);
	
	n = read(sockfd, buffer, MAXBUF);
	if (n <= 0)
		break;
	else
	{
		printf("%s", buffer);
	}
}
	//Closing the connection
	close(sockfd);		

	return 0;	
} 
