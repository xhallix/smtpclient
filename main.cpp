#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Credentials.cpp"
#include "Smtp.cpp"


// https://tools.ietf.org/html/rfc5321#section-3.1
int main(int argc, const char * argv[]) {
    // insert code here...
  
    Credentials credentials = Credentials();
    std::string username = credentials.getUsername();
    std::string password = credentials.getPassword();
    Smtp smtp_client = Smtp();
    smtp_client.new_connection("smtp.mailtrap.io", 25);
    // todo place this in a file
    smtp_client.auth_login(username, password);
    
    smtp_client.sendmail();
    
    smtp_client.close_connection();

    return 0;
}
