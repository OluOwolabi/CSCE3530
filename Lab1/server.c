/* Compilation: gcc -o server server.c
  Name: Olu Owolabi
  Course: CSCE 3530
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h> // Write this at the beginning of the code
#include <netdb.h>
#include <unistd.h> 
#include <string.h>
#define MAX_SIZE 5
#define MAX_BUFF 25000


//define record to save cache
struct record
{
   char url[256];
   int priority;
};
//data
struct record data[MAX_SIZE];
int count = 0;//number of element
int priority = 0;//priority to assign each time increase by 1
int geturl(char*url);//download url
void readfile(char*filename);//read configure file
int insertlist(char*url);//insert url to list data
int  portno = 443; /*HTTPS port number */

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, port, clilen, n;
   	struct sockaddr_in serv_addr, cli_addr; 
    char buffer[MAX_BUFF]; 
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
	   if(argc < 2)
	   {
     printf("\nPort number is missing...\n");
		   exit(0);
	   }
    readfile("list.txt");
	   //printf("list list\n");
	   sockfd = socket(AF_INET, SOCK_STREAM, 0);
	   if (sockfd < 0)
	   {
		   error(EXIT_FAILURE, 0, "ERROR opening socket");
	   }
	   bzero((char *) &serv_addr, sizeof(serv_addr));
	   port = atoi(argv[1]);
	   serv_addr.sin_family = AF_INET;
	   serv_addr.sin_addr.s_addr = INADDR_ANY;
	   serv_addr.sin_port = htons(port);
	   if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		      error(EXIT_FAILURE, 0, "ERROR binding");
	   printf("\nServer Started and listening to the port %d\n", port);
    listen(sockfd, 5);
	   while(1)
	   {
		   //Connecting with the client
		   clilen=sizeof(cli_addr);
		   newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
		   if (newsockfd < 0)
				   error(EXIT_FAILURE, 0, "ERROR on accept");
		   printf("\nClient is connected...\n");

		   //Receiving the message from the client
		   bzero(buffer,MAX_BUFF);
		   n = read(newsockfd,buffer,MAX_BUFF);
    	if(n < 0)
       error(EXIT_FAILURE, 0, "ERROR reading from socket");
    	else
    	{
      printf("\nClient has request url: %s\n", buffer);
		    //Sending the message to the client
     	if(insertlist(buffer) != 200)
     	{
     	   n = write(newsockfd, "Invalid url", strlen("Invalid url"));
     	}else
     	{
       fp = fopen(buffer, "r");
       if (fp == NULL)
          {
          //first time download
            geturl(buffer);
            fp = fopen(buffer, "r");
            if (fp == NULL)
            {
              printf("file %s is not existed!\n",buffer);
              return 0;
            }
          }
       while(len = fgets(buffer, MAX_BUFF, (FILE*) fp) > 0) {
           //printf("%s\n", buffer);
           n = write(newsockfd, buffer, strlen(buffer));
       }


       fclose(fp);
     	}
    }//else
			   //Closing the connection
			   close(newsockfd);		
	   }//while
	return 0;
}



int geturl(char*url)
{
 int response_code=500;
 struct hostent *host;
 struct sockaddr_in dest_addr;

 int n,request_len;
 FILE * pFile;
 char buff[MAX_BUFF];
 char request[256];
 int sockfd;
 static char *REQUEST_TEMPLATE= "GET / HTTP/1.1\r\nHost:%s\r\n\r\n";
 request_len = 256;
 static int require_server_auth=1;

if ( (host = gethostbyname(url)) == NULL ) {
    printf("Error: Cannot resolve hostname %s.\n",  url);
    abort();
  }
  
  
sockfd = socket(AF_INET, SOCK_STREAM, 0);
dest_addr.sin_family=AF_INET;
dest_addr.sin_port=htons(portno);
memcpy(&dest_addr.sin_addr, host->h_addr, host->h_length);
/* ---------------------------------------------------------- *
   * Zeroing the rest of the struct                             *
   * ---------------------------------------------------------- */
  memset(&(dest_addr.sin_zero), '\0', 8);

  /* ---------------------------------------------------------- *
   * Try to make the host connect here                          *
   * ---------------------------------------------------------- */
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,sizeof(struct sockaddr)) == -1 ) {
    printf("Error: Cannot connect to url %s  on portno %d.\n", url, portno);
    goto cleanup;
  }
    memset(request,0,sizeof(request));
    sprintf(request,"GET / HTTP/1.1\r\nHost:%s\r\n\r\n",url);
    /* Find the exact request_len */
    request_len=strlen(request);
    // Code for proxy server-origin server socket creation// Write this section of codes before the HTTP request code
    /* Initialize OpenSSL */
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    /* Creating SSL connection */
    SSL *conn = SSL_new(ssl_ctx);
    SSL_set_fd(conn, sockfd);
    /* SSL handshaking */
    int err = SSL_connect(conn);
    if(err != 1)
    {
       abort();
    }
    // Write the HTTP request header
    /* HTTPS request using SSL in place of write() function*/
    /* 2 nd parameter is the request string, and 3 rd parameter is the size of
    the request string */
    n = SSL_write(conn, request, strlen(request));
    /* HTTPS read using SSL in place of read() function*/
     //read one time and exit
    memset(buff,0,sizeof(buff));
    n = SSL_read(conn, buff, MAX_BUFF);
    //printf("after:%d\n",n);
    if(n > 0)
    {
     response_code = 200;
     pFile = fopen(url, "a");
     if(pFile != NULL)
     {
         fprintf(pFile,"%s",buff);
         fclose(pFile);
     }
    }
    cleanup:
    /* Closing connection */
    SSL_shutdown(conn); /* SSL connection closing */
    close(sockfd); /* Closing proxy server-origin server socket ID */
return response_code;
}     
void readfile(char*filename)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    if (fp == NULL)
       {
        exit(EXIT_FAILURE);
       }
    while ((read = getline(&line, &len, fp)) != -1) {
        line[read-2] = 0;//remove /n
        if(count < MAX_SIZE)
        {
           memset(data[count].url,0,sizeof(data[count].url));
           strcpy(data[count].url,line);
           data[count].priority=priority++;
           count++;
        }
    }

    fclose(fp);
    if (line)
        free(line);
}
int insertlist(char*url)
{
 int lowest_priority = 0;
 int i,status,response_code = 200;
 int found = 0;
 int index;
 if(url == 0)
 {
  return response_code;
 }
 for(i = 0; i < MAX_SIZE;i++)
 {
  if((strcmp(data[i].url,url) == 0)&&( data[i].priority != -1))
  {
    data[i].priority = priority;
    found = 1;
    printf("found in cached:%s\n",url);
    break;
  }
  
 }//for
 
 if(found==0)
 {
     if(count < (MAX_SIZE-1))
     {
          for(i = 0;i < MAX_SIZE;i++)
          {
            if(data[i].priority==-1)
            {
             memset(data[count].url,0,sizeof(data[count].url));
             strcpy(data[count].url,url);
             data[count].priority=priority++;
             count++;
            }
          }//for
   } else
     {
        //find lowest priority
        lowest_priority = data[0].priority;
        index = 0;
        for(i=1;i < MAX_SIZE;i++)
        {
          if(lowest_priority < data[i].priority)
          {
            index = i;
            lowest_priority = data[i].priority;
          }
        }
         //delete file
         status = remove(data[index].url);
         if(status != 0)
         {
           printf("delete file:%s unsuccessfully\n",data[index].url);
         }
         memset(data[index].url,0,sizeof(data[index].url));
         strcpy(data[index].url,url);
         data[index].priority=priority++;
     }
     //download file
     response_code = geturl(url);
     if(response_code != 200)
     {
       status = remove(data[index].url);
       memset(data[index].url,0,sizeof(data[index].url));
       data[index].priority= -1;
       count--;
     } 
 }//found
 return response_code;
}
