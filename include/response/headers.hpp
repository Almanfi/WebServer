#ifndef HEADERS_HPP
#define HEADERS_HPP
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
class Headers
{
    private:
        std::map<std::string, std::string> content_type;
        std::map<int, std::string> status_message;
        std::map<std::string, std::string> headersField;

    public:
        Headers();
        ~Headers();
        std::string getContentType(const std::string &file_name);
        void setStatusCode(int status_code);
        void setHeader(const std::string &key, const std::string &value);
        void setStatusMessage(int status_code);
        void setContentLength(int content_length);
        void setContentType(const std::string &path);
        //void setLocation(const std::string &path);
        void setConnection(const std::string &connection);
        std::string getStatusMessage(int status_code);

        std::string getHeader();
};  

#endif // HEADERS_HPP
