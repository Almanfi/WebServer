#include "../../include/response/headers.hpp"
#include <iostream>

Headers::Headers()
{
}

Headers::~Headers()
{
}

std::string Headers::getContentType(const std::string &file_name)
{
    std::string extension = "";
    for (unsigned long i = file_name.size(); i > 0; i--)
    {
        if (file_name[i] == '.')
        {
            extension = file_name.substr(i + 1);
            break;
        }
    }
    return Config::getMimeType(extension);
    // if (content_type.find(extension) != content_type.end())
    //     return content_type[extension];
    // else
    //     return "application/octet-stream";
}
void Headers::setStatusCode(int status_code)
{
    std::stringstream ss;
    ss << status_code;
    if (headersField.find(".status_code") == headersField.end())
    {
        headersField[".status_code"] = ss.str();
        setStatusMessage(status_code);
    }
}
void Headers::setStatusMessage(int status_code)
{
    // if (status_message.find(status_code) != status_message.end())
    //     headersField[".status_message"] = status_message[status_code];
    // else
    //     headersField[".status_message"] = "Internal Server Error";
    headersField[".status_message"] = Config::getStatusCode(status_code);
}

void Headers::setContentLength(int content_length)
{
    std::stringstream ss;
    ss << content_length;
    headersField["Content-Length"] = ss.str();
}

void Headers::setContentType(const std::string &path)
{
    headersField["Content-Type"] = getContentType(path);
    // if (headersField["Content-Type"].find("video") != std::string::npos || headersField["Content-Type"].find("audio") != std::string::npos)
    // {
    //     headersField["Accept-Ranges"] = "bytes";
    // }
}

void Headers::setConnection(const std::string &connection)
{
    headersField["Connection"] = connection;
}

std::string Headers::getHeader()
{
    std::string header = "HTTP/1.1 " + headersField[".status_code"] + " " + headersField[".status_message"] + "\r\n";
    for (std::map<std::string, std::string>::iterator it = headersField.begin(); it != headersField.end(); it++)
    {
        if (it->first == ".status_code" || it->first == ".status_message")
            continue;
        header += it->first + ": " + it->second + "\r\n";
    }
    header += "\r\n";
    return header;
}
std::string Headers::getStatusMessage(int status_code)
{
   return Config::getStatusCode(status_code);
}

void Headers::setHeader(const std::string &key, const std::string &value)
{
    headersField[key] = value;
}

std::string Headers::getCGIHeader()
{
    std::string header;
    std::string status_code = cgiHeaders.find("Status")->second;
    if (status_code.empty())
    {
        status_code = "200";
        header = "HTTP/1.1 " + status_code + " " + status_message[200] + "\r\n";
    }
    else
        header = "HTTP/1.1 " + status_code + " " + cgiHeaders.find("Status")->second + "\r\n";
    for (std::multimap<std::string, std::string>::iterator it = cgiHeaders.begin(); it != cgiHeaders.end(); it++)
    {
        if (it->first == "Status")
            continue;
        header += it->first + ": " + it->second + "\r\n";
    }
    return header;
}

void Headers::setCGIHeader(const std::string &key, const std::string &value)
{
    cgiHeaders.insert(std::pair<std::string, std::string>(key, value));
}
