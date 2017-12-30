#include <stdio.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


class Smtp {

private:
    int sockfd;
    char buffer[1024];
    
    
private:
    
    
    void error(const char *msg)
    {
        perror(msg);
        exit(0);
    }
    
    /**
     *  Reads socket to buffer
     */
    ssize_t read_to_buffer()
    {
        ssize_t res = read(this->sockfd, this->buffer, 1024);
        if (res < 0) {
            std::cout << "ERROR reading from socket";
            exit(0);
        }
        return res;
    }
    
    /**
     * Clears the buffer
     */
    void empty_buffer()
    {
        bzero(this->buffer,1025);
    }
    
    /**
     *  Wrapper for a socket connection
     */
    void build_connection(std::string domain, int port_no)
    {

        struct sockaddr_in serv_addr;
        struct hostent *server;
        this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->sockfd < 0)
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
        serv_addr.sin_port = htons(port_no);
        
        if (connect(this->sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            error("ERROR connecting");
        }
    }
    

    /**
     * Sends a command to the server
     * should_read : bool - On certain command we need to send more data without reading from socket
     */
    void write_command(std::string command, bool should_read = true)
    {
        ssize_t n;
        empty_buffer();
        command += '\r';
        command += '\n';
        char command_buffer[255];
        strcpy(command_buffer, command.c_str());
        n = write(sockfd,command_buffer,strlen(command_buffer));
        if (n < 0){
            error("ERROR writing to socket");
        }
        
        if (should_read) {
            n = read_to_buffer();
            if (n < 0) {
                error("ERROR reading from socket");
            }
            printf("%s\n",this->buffer);
        }
    }
    
    
    /**
     *  Encodes a string as base64
     *  https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
     */
    std::string base64_encode(char const* bytes_to_encode, size_t in_len)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        
        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                
                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }
        
        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';
            
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];
            
            while((i++ < 3))
                ret += '=';
            
        }
        
        return ret;
    }
    
    
public:
    
    /**
     *  Uses auth login functionality
     */
    void auth_login(std::string username, std::string password)
    {
        write_command("AUTH LOGIN");
        std::string uname_64 = this->base64_encode(username.c_str(), username.length());
        std::string password_64 = this->base64_encode(password.c_str(), password.length());
        write_command(uname_64);
        write_command(password_64);
    }
    
    void sendmail()
    {
        
        write_command("MAIL FROM: <foo@bar.de>");
        write_command("RCPT TO: <foo@bar.de>");
        
        write_command("DATA");
        write_command("MIME-Version: 1.0", false);
        write_command("From: foo@bar.de <foo@bar.de>", false);
        write_command("To: foo@bar.de <foo@bar.de>", false);
        write_command("Subject: Welcome", false);
        write_command("Date: Thu, 26 Oct 2017 13:10:50 +0200", false);
        write_command("\r\n", false);
        write_command("This is a test", false);
        write_command(" ", false);
        write_command(".");
        write_command("QUIT");
    }
    
    /**
     * Establish a new connection via socket
     */
    void new_connection(std::string domain, int port_no) {
        // insert code here...
        std::cout << "Starting smpt client \n";
        
        ssize_t n;
        build_connection(domain, port_no);
        
        n = read_to_buffer();
        // initial
        printf("%s\n", this->buffer);
        write_command("EHLO localhost");
    }
    
    /**
     * Close the connection
     */
    void close_connection()
    {
        close(this->sockfd);
    }

};
