#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "headers.hpp"
#include <dirent.h>
#include "../../include/response/utils.hpp"

class Request;
class ServerSocket;
class Client;

class Response
{
    private:
        std::string bufferToSend;
        std::fstream fileToSend;
        std::string locationPath;
        std::string bodyPath;
        std::string query;
        std::string uri;
        
        bool started;
        bool ended;
        bool reachedEOF;
        bool isCGI;

        struct stat buff;

        Request *request;
        ServerSocket *servSock;
        UUID *uuid;
        Headers header;
        fstream file;
        t_method method;

        int status_code;
        //----------------------CGI----------------------//
        std::string cgiOutPutFile;
        std::map<std::string, std::string> env;
        bool isCGIStarted;
        bool isCGIEnded;
        bool isHeaderSent;

        std::string tmpOutFilePath;

        int cgiFd[2];
        int bodyFd;
        pid_t cgiPid;
        int cgiStatus;
        int fdOut;
        int fdIn;
        int cgiHeaderSize;
        char **envp;
        std::clock_t cgiStartTimer;

        IHeader& requestHeaders;
        IUniqFile& body;
        IClientConf* config;
        sock_fd fd;


    public:
        Response(IHeader& requestHeaders, IUniqFile& body, IClientConf* config, int fd);
        ~Response();
        void initResponse(IClientConf* conf,int status_code);
        void sendNextChunk();
        // void setLocation();
        void sendResponse();
        void handleDirectory();
        void handleFile();
        void sendDirectory(const std::string &path);
        void sendFile(const std::string &path);
        void handleError(int error_code);
        bool isEnded();
        bool isStarted();
        // std::string generateErrorPage(int errorCode, const std::string& errorMsg);
        bool checkForValidMethod();
        bool handleRedirection();
        void handleGet();
        void handlePost();
        void handleDelete();

        // void uriParser();

        // ----------------------CGI----------------------//
        char **getEnvironmentVariables();
        bool isForCGI();
        // void executeCGI();
        bool checkGGIProcess();
        void uriParser();
        void initCGI();
        void handleCGI();
        void handleCGIResponse();
        std::string extractCGIHeaders();


};
#endif
