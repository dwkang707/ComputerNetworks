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
#include <fcntl.h> // low-level file open
#include <sys/sendfile.h> // sendfile()
#define BUF_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}
/*
void SendErrorMSG(int sock) // Error 처리
{
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char protocol[] = "HTTP/1.1 400 Bad Request\r\n";
    char servName[] = "Server:simple web server\r\n";
    char cntLen[] = "Content-length:2048\r\n";
    char cntType[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html><head><title>Computer Network</title></head>"
        "<body><font size=5><br>에러. 요청 파일이나 요청 방식 확인."
        "</font></body></html>";
    write(sock, protocol, strlen(protocol));
    write(sock, servName, strlen(servName));
    write(sock, cntLen, strlen(cntLen));
    write(sock, cntType, strlen(cntType));
    write(sock, content, strlen(content));
    close(sock);
}

char* ContentType(char* file) // Content-Type 구분
{
    char extension[BUF_SIZE];
    char filename[BUF_SIZE];
    strcpy(filename, file);
    strtok(filename, ".");
    strcpy(extension, strtok(NULL, "."));
    if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
} 

void SendData(int sock, char* ct, char* filename)
{
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char servName[] = "Server:simple web server\r\n";
    char cntLen[] = "Content-length:2048\r\n";
    char cntType[BUF_SIZE];
    char buf[BUF_SIZE];
    FILE* sendFile;
    sprintf(cntType, "Content-type:%s\r\n\r\n", ct);
    if ((sendFile = fopen(filename, "r")) == NULL) {
        SendErrorMSG(sock);
        return;
    }

    // 헤더 정보 전송
    write(sock, protocol, strlen(protocol));
    write(sock, servName, strlen(servName));
    write(sock, cntLen, strlen(cntLen));
    write(sock, cntType, strlen(cntType));

    // 요청 data 전송
    while (fgets(buf, BUF_SIZE, sendFile) != NULL)
        write(sock, buf, strlen(buf));
    close(sock); // HTTP protocol에 의해 응답 후 종료
}

void *socketThread(void *arg)
{
    int sock = *((int *)arg);
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buf[BUF_SIZE]; // 요청 메시지
    char method[BUF_SIZE]; // GET, POST 방식 저장
    char ct[BUF_SIZE]; // Content-Type 저장
    char filename[BUF_SIZE]; // 요청한 파일 이름 저장
    read(sock, buf, BUF_SIZE - 1);
    if (strstr(buf, "HTTP/") == NULL) { // HTTP 요청인지 확인
        SendErrorMSG(sock); // 현재 소켓 에러 메시지 출력
        close(sock); // HTTP 요청이 아니어서 통신 종료시킴
        exit(1); // 함수를 빠져 나옴
    }
    strcpy(method, strtok(buf, " /"));
    if (strcmp(method, "GET")) // GET 방식 요청인지 확인
        SendErrorMSG(sock); // 현재 소켓 에러 메시지 출력
    strcpy(filename, strtok(NULL, " /")); // 요청한 파일 확인
    strcpy(ct, ContentType(filename)); // Content-Type 확인
    SendData(sock, ct, filename); // 응답
}
*/
int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno; // port number
    int fd; // file descriptor -> html, jpg, pdf, gif, mp3 등의 확장자를 가진 파일을 읽으려고 사용
    FILE *fp; // high-level file descriptor
    socklen_t clilen;
    pid_t pid; // fork()시 pid가 저장됌 -> -1: 오류, 0: child process, 0 < pid: parent process
    char c; // image file fgetc를 위해 사용
     
    char buffer[BUF_SIZE];
    char buf[1000];
    char imageBuf[20000];
    char motionBuf[1000000];
    char pdfBuf[24000];
    // Only this line has been changed. Everything is same.
    char *responseHeader = "HTTP/1.1 200 OK"; //\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
     
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
        if (newsockfd < 0) {
            error("ERROR on accept");
            continue;
        }
         
        bzero(buffer, BUF_SIZE);
        bzero(buf, 1000);
        bzero(imageBuf, 20000);
        bzero(motionBuf, 1000000);
        bzero(pdfBuf, 24000);
        //n = read(newsockfd, buffer, BUF_SIZE - 1); //Read is a block function. It will read at most 255 bytes
        //if (n < 0)
            //error("ERROR reading from socket");
        pid = fork();
        if (pid < 0) { // fork error
            perror("fork error");
            exit(1);
        }
        else if (pid == 0) {// child process
            close(sockfd);
            //memset(buffer, 0, 2048);
            read(newsockfd, buffer, 2047);
            printf("%s\n", buffer);

            // strncmp(A, B, size_t n): A와 B 문자열을 n 만큼 비교 -> A가 길면 0보다 큰 값, B가 길면 0보다 작은 값, 같으면 0 return
            if (!strncmp(buffer, "GET /index.html", 15)) {
                fd = open("index.html", O_RDONLY);
                //fp = fopen("index.html", "r");
                //write(fd, newsockfd, 10);
                read(fd, buf, 400);
                //sendfile(newsockfd, fp, NULL, 400);
                
                write(newsockfd, responseHeader, strlen(responseHeader));
                write(newsockfd, "\nContent-Type: text/html", strlen("\nContent-Type: text/html"));
                write(newsockfd, "\nContent-Length: ", strlen("\nContent-Length: "));
                write(newsockfd, (char*)strlen(buf), strlen(buf));
                write(newsockfd, "\n\n", strlen("\n\n"));
                write(newsockfd, buf, strlen(buf));
                close(fd);
            }
            else if (!strncmp(buffer, "GET /image.jpg", 14)) {
                //fd = open("image.jpg", O_RDONLY);
                //read(fd, imageBuf, 19999);
                fp = fopen("image.jpg", "r");
                //sendfile(newsockfd, fd, NULL, 35000);
                write(newsockfd, responseHeader, strlen(responseHeader));
                write(newsockfd, "\nContent-Type: image/jpeg", strlen("\nContent-Type: image/jpeg"));
                write(newsockfd, "\nContent-Length: ", strlen("\nContent-Length: "));
                write(newsockfd, (char*)strlen(imageBuf), strlen(imageBuf));
                write(newsockfd, "\n\n", strlen("\n\n"));
                //write(newsockfd, "imageBuf", strlen(imageBuf));
                while ((c = fgetc(fp)) != EOF)
                    write(newsockfd, (char*)c, 1);
                fclose(fp);
            }
            else if (!strncmp(buffer, "GET /motion.gif", 15)) {
                fd = open("motion.gif", O_RDONLY);
                read(fd, motionBuf, 999999);
                //sendfile(newsockfd, fd, NULL, 1000000);
                write(newsockfd, responseHeader, strlen(responseHeader));
                write(newsockfd, "\nContent-Type: image/gif", strlen("\nContent-Type: image/gif"));
                write(newsockfd, "\nContent-Length: ", strlen("\nContent-Length: "));
                write(newsockfd, (char*)strlen(motionBuf), strlen(motionBuf));
                write(newsockfd, "\n\n", strlen("\n\n"));
                write(newsockfd, motionBuf, strlen(motionBuf));
                close(fd);
            }
            else if (!strncmp(buffer, "GET /pdf_file.pdf", 17)) {
                fd = open("pdf_file.pdf", O_RDONLY);
                read(fd, pdfBuf, 23999);
                
                write(newsockfd, responseHeader, strlen(responseHeader));
                write(newsockfd, "\nContent-Type: application/pdf", strlen("\nContent-Type: application/pdf"));
                write(newsockfd, "\nContent-Length: ", strlen("\nContent-Length: "));
                write(newsockfd, (char*)strlen(pdfBuf), strlen(pdfBuf));
                write(newsockfd, "\n\n", strlen("\n\n"));
                write(newsockfd, pdfBuf, strlen(pdfBuf));
                //sendfile(newsockfd, fd, NULL, 21000);
                close(fd);
            }
            /*
            else if (!strncmp(buffer, "GET /music.mp3", 16)) {
                fd = open("music.mp3", O_RDONLY);
                sendfile(newsockfd, fd, NULL, 35000);
                close(fd);
            }
            */
            else {
                n = write(newsockfd, buffer, 18); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
                if (n < 0)
                    error("ERROR writing to socket");
            }
        }
        
        //n = write(newsockfd, buffer, 18); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send
        //if (n < 0)
            //error("ERROR writing to socket");
         
        close(newsockfd);
    }  
    return 0;
}
