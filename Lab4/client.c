//Compilation: gcc -o client client.c
//Name: Olu Owolabi
//Course: CSCE 3530

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DATA_SEG_SIZE 128		//set the data lenght to be transferred

//initialize TCP header struct
struct tcp_hdr{
                unsigned short int src;
                unsigned short int des;
                unsigned int seq;
                unsigned int ack;
                unsigned short int hdr_flags;
                unsigned short int rec;
                unsigned short int cksum;
                unsigned short int ptr;
                unsigned int opt;
                char data[DATA_SEG_SIZE];
              };

unsigned short int computeChecksum(struct tcp_hdr tcp_seg); //computes checksum
int verifyChecksum(struct tcp_hdr tcp_seg);	//verifies checksum
void printAndWriteToFile(struct tcp_hdr tcp_seg);
void logMessage(char* message);
void logHeader(struct tcp_hdr tcp_seg);
char* readDataFromFile(long int offset);	//read the data to be sent from file

int main(int argc,char **argv)
{
    struct tcp_hdr client_tcp_seg, server_tcp_seg;
    int sockfd, n, tmpres, portno;
    unsigned short int segment_arr[76];
    char* buffer;
	struct hostent *server;	//stores the server ip
    

    int len = sizeof(struct sockaddr);

    struct sockaddr_in servaddr, clientadrr;

    if (argc < 3){
		printf("Usage <serverip> <Port_Number> \n");
		exit(0);
	}

    portno = atoi(argv[2]);//set port number from user input
    sockfd=socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0){
		printf("ERROR opening socket");
	}
	server = gethostbyname(argv[1]);//set serverip from user input
	if(server == NULL){
		fprintf(stderr,"Error, no such host");
		exit(0);
	}
	
	bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;

	servaddr.sin_port=htons(portno);
    // hard coded the tcp server's IP address
    memcpy(&servaddr.sin_addr, server->h_addr, server->h_length);

    // establish a socket connection with the server
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

//Retrieve the locally-bound name of the specified socket and store it in the sockaddr structure
if (getsockname(sockfd, (struct sockaddr *)&clientadrr, &len) == -1)
    perror("getsockname");

// Creating the initial tcp segment
  //memset(client_tcp_seg.data, 0, 4);
  client_tcp_seg.src = (unsigned short int) ntohs(clientadrr.sin_port);
  client_tcp_seg.des = atoi(argv[1]);
  client_tcp_seg.seq = 11; // arbitary sequence number
  client_tcp_seg.ack = 0;
  client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b000010;
  client_tcp_seg.rec = 0;
  client_tcp_seg.ptr = 0;
  client_tcp_seg.opt = 0;
  client_tcp_seg.cksum = computeChecksum(client_tcp_seg);

 logMessage("############## Demonstrating the TCP 3-way handshake ################\n");
 logMessage("#####################################################################\n\n");
 logMessage("************** Initial Client TCP header, sending it to server ****************\n");
	// print the header
	printAndWriteToFile(client_tcp_seg);

    //Write  the tcp segment to the socket
    memcpy(segment_arr, &client_tcp_seg, 152); //Copying 152 bytes

    if( write(sockfd , segment_arr , sizeof(segment_arr)) < 0)
    {
        puts("Write failed");
        return 1;
    }

	read(sockfd, segment_arr,sizeof(segment_arr));
	memcpy(&server_tcp_seg, segment_arr, 152); //Copying 152 bytes
	logMessage("************** Response header received from Server ****************\n");
	printAndWriteToFile(server_tcp_seg);
	if(!verifyChecksum(server_tcp_seg)) {
		logMessage("Corrupted data\n");
	}
	else if(server_tcp_seg.ack == (client_tcp_seg.seq + 1)) {
		logMessage("Received ACK - x, Connection successfull! Server is live.\n");
	}
  client_tcp_seg.seq++;
  client_tcp_seg.ack = server_tcp_seg.seq + 1;
  client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b010010;
  client_tcp_seg.cksum = computeChecksum(client_tcp_seg);

	logMessage("************** Modified Client TCP-header, sending it to server ****************\n");
	// print the header
	printAndWriteToFile(client_tcp_seg);

    memcpy(segment_arr, &client_tcp_seg, 152); //Copying 152 bytes
    //Write  the tcp segment to the socket
    if( write(sockfd , segment_arr , sizeof(segment_arr)) < 0)
    {
        puts("Write failed");
        return 1;
    }

 logMessage("###################### Finished the 3-way handshake ###################\n\n");
 
 
 
 
    client_tcp_seg.seq = 0;
    client_tcp_seg.ack = 0;
    client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b000010;
    buffer = readDataFromFile(client_tcp_seg.ack);
    strncpy(client_tcp_seg.data, buffer, 128);
    client_tcp_seg.cksum = computeChecksum(client_tcp_seg);
 
 while(client_tcp_seg.data == NULL | strlen(client_tcp_seg.data) != 0) {
    memcpy(segment_arr, &client_tcp_seg, 152); //Copying 152 bytes
    if( write(sockfd , segment_arr , sizeof(segment_arr)) < 0) {
        puts("Write failed");
        return 1;
    }
    
    printAndWriteToFile(client_tcp_seg);
    
    read(sockfd, segment_arr,sizeof(segment_arr));
	memcpy(&server_tcp_seg, segment_arr, 152); //Copying 152 bytes
    
    printAndWriteToFile(server_tcp_seg);
    
    if(!verifyChecksum(server_tcp_seg)) {
		logMessage("Corrupted data\n");
	}
    
    client_tcp_seg.seq = server_tcp_seg.ack;
    client_tcp_seg.ack = server_tcp_seg.seq + 1;
    client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b000010;
    buffer = readDataFromFile(server_tcp_seg.ack);
    strncpy(client_tcp_seg.data, buffer, 128);
    client_tcp_seg.cksum = computeChecksum(client_tcp_seg);
     
 }
 

 logMessage("################ Demonstrating the closing of TCP connection ###################\n");
 logMessage("################################################################################\n\n");

  client_tcp_seg.seq++;
  client_tcp_seg.ack = 0;
  client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b000001; // setting the FIN bit to 1
  client_tcp_seg.cksum = computeChecksum(client_tcp_seg);

	logMessage("************** Connection closing header, sending it to server ****************\n");
	// print the header
	printAndWriteToFile(client_tcp_seg);

    //Write  the tcp segment to the socket
    memcpy(segment_arr, &client_tcp_seg, 152); //Copying 152 bytes

    if( write(sockfd , segment_arr , sizeof(segment_arr)) < 0)
    {
        puts("Write failed");
        return 1;
    }

	read(sockfd, segment_arr,sizeof(segment_arr));
	memcpy(&server_tcp_seg, segment_arr, 152); //Copying 152 bytes
	logMessage("************** Response header received from Server ****************\n");
	printAndWriteToFile(server_tcp_seg);
	if(!verifyChecksum(server_tcp_seg)) {
		logMessage("Corrupted data\n");
	}  else
                logMessage("Checksum verification successfull!\n");

	read(sockfd, segment_arr,sizeof(segment_arr));
	memcpy(&server_tcp_seg, segment_arr, 152); //Copying 152 bytes
	logMessage("************** Finish header received from Server ****************\n");
	printAndWriteToFile(server_tcp_seg);
	if(!verifyChecksum(server_tcp_seg)) {
		logMessage("Corrupted data\n");
	}  else
                logMessage("Checksum verification successfull!\n");

  client_tcp_seg.seq++;
  client_tcp_seg.ack = server_tcp_seg.seq + 1;
  client_tcp_seg.hdr_flags = ((sizeof(client_tcp_seg)-DATA_SEG_SIZE)/4 << 12) | 0b010010;
  client_tcp_seg.cksum = computeChecksum(client_tcp_seg);

	logMessage("************** Final ACK for closing the connection, sending it to server ****************\n");
	// print the header
	printAndWriteToFile(client_tcp_seg);

    //Write  the tcp segment to the socket
    memcpy(segment_arr, &client_tcp_seg, 152); //Copying 152 bytes

    if( write(sockfd , segment_arr , sizeof(segment_arr)) < 0)
    {
        puts("Write failed");
        return 1;
    }

    logMessage("###################### Finished closing the TCP connection ###################\n\n");

   close(sockfd);

   return 0;

}

/**
This function computes the checksum from the tcp header passed to it as an argument.
Returns the checksum value.
*/
unsigned short int computeChecksum(struct tcp_hdr tcp_seg) {

  unsigned short int cksum_arr[76];
  unsigned int i,sum=0, cksum;
  tcp_seg.cksum = 0; // reset the checksum field to zero

  memcpy(cksum_arr, &tcp_seg, 152); //Copying 152 bytes
 
  for (i=0;i<76;i++)               // Compute sum
   sum = sum + cksum_arr[i];

  cksum = sum >> 16;              // Fold once
  sum = sum & 0x0000FFFF; 
  sum = cksum + sum;

  cksum = sum >> 16;             // Fold once more
  sum = sum & 0x0000FFFF;
  cksum = cksum + sum;


  /* XOR the sum for checksum */
  return (0xFFFF^cksum);
}

/**
This function verifies if the checksum is correct and returns 1 if it is.
It returns 0 if checksum doesn't match indicating the corrupted data.
*/
int verifyChecksum(struct tcp_hdr tcp_seg) {
	return (computeChecksum(tcp_seg) == tcp_seg.cksum);
}

/**
* This function prints the output to the console.
*/
void printAndWriteToFile(struct tcp_hdr tcp_seg) {

  printf("Src : %d\n", tcp_seg.src); // Printing all values
  printf("Dest : %d\n", tcp_seg.des);
  printf("Seq : %d\n", tcp_seg.seq);
  printf("Ack : %d\n", tcp_seg.ack);
  printf("Header Flags : 0x%04X\n", tcp_seg.hdr_flags);
  printf("Rec : 0x%04X\n", tcp_seg.rec);
  printf("Checksum : 0x%04X\n", tcp_seg.cksum);
  printf("Ptr : 0x%04X\n", tcp_seg.ptr);
  printf("Optional : 0x%08X\n\n", tcp_seg.opt);
  // log the header to a file
  logHeader(tcp_seg);

}

/**
* This function prints the message to console and logs it to the file "client.txt"
*
*/
void logMessage(char* message) {
  // print the message to the console
  printf("%s",message);
  FILE *logFile = fopen("client.txt", "a+");
  // Writing all values to a file
  fputs(message, logFile);
  fclose(logFile);

}

/**
* This function logs the tcp header to the file "client.txt"
*
*/
void logHeader(struct tcp_hdr tcp_seg) {

  FILE *logFile = fopen("client.txt", "a+");
  // Writing all values to a file
  fprintf(logFile,"Src : %d\n", tcp_seg.src);
  fprintf(logFile,"Dest : %d\n", tcp_seg.des);
  fprintf(logFile,"Seq : %d\n", tcp_seg.seq);
  fprintf(logFile,"Ack : %d\n", tcp_seg.ack);
  fprintf(logFile,"Header Flags : 0x%04X\n", tcp_seg.hdr_flags);
  fprintf(logFile,"Rec : 0x%04X\n", tcp_seg.rec);
  fprintf(logFile,"Checksum : 0x%04X\n", tcp_seg.cksum);
  fprintf(logFile,"Ptr : 0x%04X\n", tcp_seg.ptr);
  fprintf(logFile,"Optional : 0x%08X\n\n", tcp_seg.opt);

  fclose(logFile);
}

/**
 * @param offset
 * @return pointer to the string read from the file
 */
char* readDataFromFile(long int offset) {
    char *buffer = NULL;
    FILE *fp = fopen("data_client.txt", "r");
	/* Open your_file in read-only mode */
	if(fp != NULL) {
		fseek(fp, offset, SEEK_SET); /* Go to offset position of the file */
        buffer = malloc((DATA_SEG_SIZE) + 1); /* size + 1 byte for the \0 */
		fread(buffer, DATA_SEG_SIZE, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */
        buffer[DATA_SEG_SIZE] = '\0';
		fclose(fp);
	}

	return buffer;
}