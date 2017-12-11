//
//  main.cpp
//  mailclient

//  Copyright © 2017 challmann. All rights reserved.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Smtp.cpp"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

ssize_t read_to_buffer(int sockfd, char buffer[1024])
{
    ssize_t res = read(sockfd,buffer,1024);
    if (res < 0) {
        std::cout << "ERROR reading from socket";
        exit(0);
    }
    return res;
}

int build_connection()
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = 25;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("smtp.mailtrap.io");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    return sockfd;
}

void empty_buffer(char buffer[1024])
{
    bzero(buffer,1025);
}

void write_command(std::string command, char buffer[1024] , int sockfd)
{
    ssize_t n;
    empty_buffer(buffer);
    command += '\n';
    char command_buffer[255];
    strcpy(command_buffer, command.c_str());
    n = write(sockfd,command_buffer,strlen(command_buffer));
    if (n < 0){
        error("ERROR writing to socket");
    }
    n = read_to_buffer(sockfd,buffer);
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);
}

// https://tools.ietf.org/html/rfc5321#section-3.1
int main(int argc, const char * argv[]) {
    // insert code here...
  
    Smtp smtp_client = Smtp();
    smtp_client.new_connection("smtp.mailtrap.io", 25);
    smtp_client.auth_login("Username", "password");
    smtp_client.close_connection();

    return 0;
}
