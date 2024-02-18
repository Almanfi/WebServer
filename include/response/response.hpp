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

class Request;
class ServerSocket;
class Client;

typedef struct s_location
{
    map<int, std::string> error_page;
    vector<t_method> methods;
    vector<string> index;
    string root;
    string return_url;
    int return_code;
    bool autoindex;
    bool allow_upload;
    std::string upload_path;
    bool allow_CGI; 
    std::string CGI_path;
    std::string CGI_timeout;

} t_location;

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
        t_location location;
        //----------------------CGI----------------------//
        std::string cgiOutPutFile;
        std::map<std::string, std::string> env;
        bool isCGIStarted;
        bool isCGIEnded;
        bool isHeaderSent;

        FILE *tmpout;
        UUID newUUID;
        UniqFile CGItmpFile;
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
        void initResponse(IClientConf* conf);
        void sendNextChunk();
        void setLocation();
        void sendResponse();
        void handleDirectory();
        void handleFile();
        void sendDirectory(const std::string &path);
        void sendFile(const std::string &path);
        void handleError(int error_code);
        bool isEnded();
        bool isStarted();
        std::string generateErrorPage(int errorCode, const std::string& errorMsg);
        std::string generateDirectoryListingPage(DIR* dir);
        bool checkForValidMethod();
        bool handleRedirection();
        void handleGet();
        void handlePost();
        void handleDelete();
        std::string joinPath(const std::string &path1, const std::string &path2);
        std::string generateListHTML(struct dirent* entry);
        std::string decodingURI(const std::string &uri);
        // void uriParser();

        // ----------------------CGI----------------------//
        char **getEnvironmentVariables();
        // void executeCGI();
        bool checkGGIProcess();
        void uriParser();
        void initCGI();
        void handleCGI();
        void handleCGIResponse();
        std::string extractCGIHeaders();


};
#endif
