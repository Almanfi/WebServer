#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "socket.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <errno.h>


class Request;
class ServerSocket;
class Client;

class CGI 
{
    private: 
        // Request *req;
        // std::map<std::string, std::string> headers;
        std::map<std::string, std::string> env;
        std::string bodyPath;
        std::string CGI_path;
        std::string uri;
        struct stat bodyStat;




    public:
        CGI();
        CGI(std::string bodyPath, std::string CGI_path, std::string uri);
        ~CGI();
        void setEnvironmentVariables();
        int ExecuteCGI();
        char **getEnv();


    
};