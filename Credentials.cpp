#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

class Credentials {
    
private:
    const std::string auth_file = "auth.txt";
    
    std::string username = "";
    std::string password = "";
    
public:
    
    /**
     * Class to handles credentials
     */
    Credentials()
    {
        std::ifstream file_to_read(auth_file);
        
        if (!file_to_read) {
            std::cout << "Cannot read from file" << std::endl;
            exit(0);
        }
        
        std::string linebuffer;
        int line_counter = 0;
        while (file_to_read && getline(file_to_read, linebuffer)){
            if (linebuffer.length() == 0) {
                continue;
            }
            if (line_counter == 0)
            {
                this->username = linebuffer;
            }
            if (line_counter == 1)
            {
                this->password = linebuffer;
            }
            line_counter++;
        }
    }
    
    /**
     * Get the password
     */
    std::string getPassword()
    {
        
        return this->password;
    }
    
    /**
     * Get the username
     */
    std::string getUsername()
    {
        return this->username;
    }
    
};
