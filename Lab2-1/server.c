//Compilation: gcc -o server server.c
//Name: Olu Owolabi
//Course: CSCE 3530

#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 1024

int main(int argc, char *argv[]){
	
	int sockfd, newsockfd, clilen, port, n;
	char buf[MAXLINE], req_buf[MAXLINE],
	     hostname[MAXLINE], time_[20];
	char blacklist_buf[] = "website is blacklisted";
	struct sockaddr_in serv_addr, cli_addr;
	FILE *list_fp, *blacklist_fp;			//files to store cache and blacklisted websites
	pid_t pid;
	
	//checks the argument passed if it has the port number
	if(argc < 2){	
		printf("\nPort number is missing...\n");
		exit(1);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd< 0){
		printf("Error opening socket");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	port = atoi(argv[1]);
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	//bind the connection to the client
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Error on binding");
		exit(1);
	}

	listen(sockfd, 10);
	while(1){

		pid = fork();
		if(pid == 0){
			
			printf("Server Started");
			//Connecting to the client
			clilen = sizeof(cli_addr);
			//accept connecion from the client
			newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
			if( newsockfd < 0){
				printf("Error failed to connect to client\n");
				exit(1);
			}
			
			printf("\nClient is connected...\n");
			memset(buf, 0, sizeof(buf));
			//Receiving the message from the client
			n = read(newsockfd, buf, sizeof(buf));
			if(n < 0){
				printf("Error: reading from socket");
				exit(1);
			}
			memset(hostname, 0, sizeof(hostname));
			strcpy(hostname, buf);
			printf("\nurl requested: <%s>\n", hostname);

			memset(buf, 0, sizeof(buf));
			
			//check if the URL requested has been blacklisted
			blacklist_fp = fopen("blacklist.txt", "r");
			while((fscanf(blacklist_fp, "%s %*s %*s", buf)) != EOF){
				if(strcmp(buf, hostname) == 0){
					n = write(newsockfd, blacklist_buf,
						strlen(blacklist_buf));
					fclose(blacklist_fp);
				}
			}
			int websockfd, portnum;
			struct sockaddr_in dest_addr;
			struct hostent *host;
			portnum = 80;//port number to connect to the web server	
			
			//create a socket to connect to the webserver requested by the client
			if((websockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				printf("ERROR: creating web socket");
				exit(1);
			}
			
			
			if((host = gethostbyname(hostname)) == NULL){
				herror("gethostbyname");
				exit(1);
			}
			memset(&dest_addr, 0 , sizeof(dest_addr));

			
			dest_addr.sin_family = AF_INET;
			bcopy((char *)host->h_addr,
				(char *)&dest_addr.sin_addr.s_addr,
				host->h_length);
			dest_addr.sin_port = htons(portnum);

			//make the reuest to the webserver on behalf of the client
			printf("\nConnecting to web server...\n");
			if(connect(websockfd, (struct sockaddr *)&dest_addr,
			sizeof(dest_addr)) < 0){
				printf("ERROR: connecting to web socket");
				exit(1);
			}
			
			
			//Use the GET to pass on the client request
			memset(req_buf, 0, sizeof(req_buf));
			strcpy(req_buf, "GET / HTTP/1.0\r\nHost: ");
			strcat(req_buf, hostname);
			strcat(req_buf, "\r\nConnection: Close\r\n\r\n");

			//sending the client request to the webserver
			printf("Sending url request to web server...\n");
			n = write(websockfd, req_buf, strlen(req_buf));
			if(n < 0){
				printf("ERROR: writing to web socket");
				exit(1);
			}

			
			memset(req_buf, 0, sizeof(req_buf));

			//getting the web server response
			printf("Reading response from web server...\n");
			n = read(websockfd, req_buf, sizeof(req_buf));
			if(n < 0){
				printf("ERROR: reading from web socket");
				exit(1);
			}

			close(websockfd);	
			time_t now = time(NULL);
			strftime(time_, 20, "%Y%m%d%H%M%S", localtime(&now));
			list_fp = fopen("list.txt", "a");
			fprintf(list_fp, "%s %s\n", hostname, time_);
			fclose(list_fp);
			
			//sending the response to the client request
			printf("Forwarding response to client...\n\n");
			n = write(newsockfd, req_buf, strlen(req_buf));
			if(n < 0){
				printf("ERROR: writing to client socket");
				exit(1);
			}

			close(newsockfd);		//close connection

		}

		
		else{
			wait((int *)0);	
		}

	}

return 0;
}
