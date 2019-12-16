/* Compilation: gcc -o client client.c
   
*/

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

	if (argc < 2)
	{
		printf("\nPort number is missing\n");
		exit(0);
	}

	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error(EXIT_FAILURE, 0, "ERROR opening socket");
	server = gethostbyname("129.120.151.95"); //IP address of server

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
