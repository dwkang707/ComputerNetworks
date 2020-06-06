/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 1024;
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno; // port number
    socklen_t clilen;
     
    char buffer[BUF_SIZE];
     
    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;
     
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     
    /*Create a new socket
      AF_INET: Address Domain is Internet 
      SOCK_STREAM: Socket Type is STREAM Socket */
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        error("ERROR opening socket");
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); //atoi converts from String to Integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
        error("ERROR on binding");
     
    listen(sockfd, 5); // Listen for socket connections. Backlog queue (connections to wait) is 5
     
    clilen = sizeof(cli_addr);
    
    while (1) {
        /*accept function: 
          1) Block until a new connection is established
          2) the new socket descriptor will be used for subsequent communication with the newly connected client.
        */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
         
        bzero(buffer, BUF_SIZE);
        //n = read(newsockfd, buffer, BUF_SIZE - 1); //Read is a block function. It will read at most 255 bytes
        //if (n < 0)
            //error("ERROR reading from socket");
        while (read(newsockfd, buffer, BUF_SIZE - 1) != 0) {
            printf("%s\n", buffer);
            n = write(newsockfd, buffer, 18); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
            if (n < 0)
                error("ERROR writing to socket");
        }
        
        //n = write(newsockfd, buffer, 18); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
        //if (n < 0)
            //error("ERROR writing to socket");
         
        close(newsockfd);
    }
    close(sockfd);   
     
    return 0;
}
