// #ifndef RESPONSE_HPP
// #define RESPONSE_HPP

// #include <string>
// #include <map>
// #include <vector>
// #include <iostream>
// #include <sstream>
// #include <fstream>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include "socket.hpp"
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <dirent.h>
// #include "headers.hpp"
// #include <dirent.h>

// class Request;
// class ServerSocket;
// class Client;
// typedef struct s_location
// {
//     map<int, std::string> error_page;
//     vector<t_method> methods;
//     vector<string> index;
//     string root;
//     string return_url;
//     int return_code;
//     bool autoindex;

// } t_location;

// class Response
// {
//     private:
//         bool started;
//         bool ended;
//         bool reachedEOF;
//         IRequest *request;
//         ServerSocket *servSock;
//         UUID *uuid;
//         fstream file;
//         sock_fd fd;
//         t_method method;
//         string uri;
//         t_location location;
//         struct stat buff;
//         Headers header;
//         std::string bufferToSend;
//         std::fstream fileToSend;
//         std::string locationPath;
//     public:
//         Response();
//         ~Response();
//         void initResponse(Client *client);
//         void sendNextChunk();
//         void setLocation();
//         void sendResponse();
//         void handleDirectory();
//         void handleFile();
//         void sendDirectory(const std::string &path);
//         void sendFile(const std::string &path);
//         void handleError(int error_code);
//         bool isEnded();
//         bool isStarted();
//         std::string generateErrorPage(int errorCode, const std::string& errorMsg);
//         std::string generateDirectoryListingPage(DIR* dir);
//         bool checkForValidMethod();
//         bool handleRedirection();
//         void handleGet();
//         void handlePost();
//         void handleDelete();
//         std::string joinPath(const std::string &path1, const std::string &path2);
//         std::string generateListHTML(struct dirent* entry);
// };
// #endif