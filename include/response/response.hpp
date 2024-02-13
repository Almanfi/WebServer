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

} t_location;

class IHeader;


//================================================== RESPONSE STRATEGY
class IResponseStrategy {
public:
    virtual ~IResponseStrategy() {};
    virtual void sendResponse() = 0;
};

class ErrorResponseStrategy : public IResponseStrategy {
private:
    int error_code;
public:
    ErrorResponseStrategy(int error_code);
    virtual ~ErrorResponseStrategy() {};
    virtual void sendResponse();
};

class RedirectionResponseStrategy : public IResponseStrategy {
private:
    std::string location;
    int status_code;
public:
    RedirectionResponseStrategy(const std::string& location, int status_code);
    virtual ~RedirectionResponseStrategy() {};
    virtual void sendResponse();
};

class GetResponseStrategy : public IResponseStrategy {
private:
    string              path;
    string              uri;
    IClientConf&        config;
    IResponseStrategy*  strategy;
    void setStrategy();
    void setDirectoryStrategy();
public:
    GetResponseStrategy(const string& path, const string& uri,
                                        IClientConf& config);
    virtual ~GetResponseStrategy() {};
    virtual void sendResponse();
};

class GetFileResponseStrategy : public IResponseStrategy {
private:
    const string& path;
    size_t sentSize;
public:
    GetFileResponseStrategy(const std::string& path);
    virtual ~GetFileResponseStrategy() {};
    virtual void sendResponse();
};

class GetDirectoryResponseStrategy : public IResponseStrategy {
private:
    const string& path;
    const string& uri;
    size_t sentSize;
    DIR* dir;
    string header;
    string generateDirectoryListingPage() const;
    string generateListHTML(struct dirent* entry) const;
public:
    GetDirectoryResponseStrategy(const std::string& path,
                            DIR* dir, const string& uri);
    virtual ~GetDirectoryResponseStrategy() {};
    virtual void sendResponse();
};

class PostResponseStrategy : public IResponseStrategy {
private:
    IResponse* response;
    std::string path;
public:
    PostResponseStrategy(IResponse* response, const std::string& path);
    virtual ~PostResponseStrategy() {};
    virtual void sendResponse();
};

class DeleteResponseStrategy : public IResponseStrategy {
private:
    IResponse* response;
    std::string path;
public:
    DeleteResponseStrategy(IResponse* response, const std::string& path);
    virtual ~DeleteResponseStrategy() {};
    virtual void sendResponse();
};
//================================================== RESPONSE STRATEGY

class IResponse {
public:
    virtual ~IResponse() {};
    virtual void sendResponse() = 0;
};

class ResponseA : public IResponse {
private:
    bool started;
    bool ended;
    bool reachedEOF;
    ISBuffer&       buffer;
    IUniqFile&      requestBody;
    IHeader&        RequestHeader;
    IClientConf&    config;
    sock_fd fd;
    t_method method;
    string uri;
    t_location location;
    fstream file;
    struct stat buff; // this is for gettign the file stats
    Headers header;
    std::string bufferToSend;
    std::fstream fileToSend;
    std::string locationPath;
    IResponseStrategy* strategy;

    string createPath();

public:
    ResponseA(ISBuffer& buffer, IUniqFile& file,
            IHeader& headers, IClientConf& config);
    virtual ~ResponseA() {};
    virtual void sendResponse();

    void setStrategy();

    void initResponse(Client *client);
    bool isEnded();
    bool isStarted();
    void sendNextChunk();
    void setLocation();
    void sendResponse();
    void handleDirectory();
    void handleFile();
    void sendDirectory(const std::string &path);
    void sendFile(const std::string &path);
    void handleError(int error_code);
    std::string generateErrorPage(int errorCode, const std::string& errorMsg);
    std::string generateDirectoryListingPage(DIR* dir);
    bool checkForValidMethod();
    bool handleRedirection();
    void handleGet();
    void handlePost();
    void handleDelete();
    std::string joinPath(const std::string &path1, const std::string &path2);
    std::string generateListHTML(struct dirent* entry);
};

class Response
{
    private:
        bool started;
        bool ended;
        bool reachedEOF;
        Request *request;
        ServerSocket *servSock;
        UUID *uuid;
        fstream file;
        sock_fd fd;
        t_method method;
        string uri;
        t_location location;
        struct stat buff;
        Headers header;
        std::string bufferToSend;
        std::fstream fileToSend;
        std::string locationPath;
    public:
        Response();
        ~Response();
        void initResponse(Client *client);
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
};

#endif