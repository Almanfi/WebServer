#include "../../include/response/headers.hpp"
#include <iostream>

Headers::Headers()
{
    content_type["html"] = "text/html";
    content_type["htm"] = "text/html";
    content_type["shtml"] = "text/html";
    content_type["css"] = "text/css";
    content_type["xml"] = "text/xml";
    content_type["rss"] = "text/xml";
    content_type["gif"] = "image/gif";
    content_type["jpeg"] = "image/jpeg";
    content_type["jpg"] = "image/jpeg";
    content_type["js"] = "application/x-javascript";
    content_type["txt"] = "text/plain";
    content_type["htc"] = "text/x-component";
    content_type["mml"] = "text/mathml";
    content_type["png"] = "image/png";
    content_type["ico"] = "image/x-icon";
    content_type["jng"] = "image/x-jng";
    content_type["bmp"] = "image/bmp";
    content_type["svg"] = "image/svg+xml";
    content_type["svgz"] = "image/svg+xml";
    content_type["tif"] = "image/tiff";
    content_type["tiff"] = "image/tiff";
    content_type["wbmp"] = "image/vnd.wap.wbmp";
    content_type["avif"] = "image/avif";
    content_type["jar"] = "application/java-archive";
    content_type["war"] = "application/java-archive";
    content_type["ear"] = "application/java-archive";
    content_type["hqx"] = "application/mac-binhex40";
    content_type["pdf"] = "application/pdf";
    content_type["cco"] = "application/x-cocoa";
    content_type["jardiff"] = "application/x-java-archive-diff";
    content_type["jnlp"] = "application/x-java-jnlp-file";
    content_type["run"] = "application/x-makeself";
    content_type["pl"] = "application/x-perl";
    content_type["pm"] = "application/x-perl";
    content_type["prc"] = "application/x-pilot";
    content_type["pdb"] = "application/x-pilot";
    content_type["rar"] = "application/x-rar-compressed";
    content_type["rpm"] = "application/x-redhat-package-manager";
    content_type["sea"] = "application/x-sea";
    content_type["swf"] = "application/x-shockwave-flash";
    content_type["sit"] = "application/x-stuffit";
    content_type["tcl"] = "application/x-tcl";
    content_type["tk"] = "application/x-tcl";
    content_type["der"] = "application/x-x509-ca-cert";
    content_type["pem"] = "application/x-x509-ca-cert";
    content_type["crt"] = "application/x-x509-ca-cert";
    content_type["xpi"] = "application/x-xpinstall";
    content_type["zip"] = "application/zip";
    content_type["deb"] = "application/octet-stream";
    content_type["bin"] = "application/octet-stream";
    content_type["exe"] = "application/octet-stream";
    content_type["dll"] = "application/octet-stream";
    content_type["dmg"] = "application/octet-stream";
    content_type["eot"] = "application/octet-stream";
    content_type["iso"] = "application/octet-stream";
    content_type["img"] = "application/octet-stream";
    content_type["msi"] = "application/octet-stream";
    content_type["msp"] = "application/octet-stream";
    content_type["msm"] = "application/octet-stream";
    content_type["mp3"] = "audio/mpeg";
    content_type["mp4"] = "video/mp4";
    content_type["ra"] = "audio/x-realaudio";
    content_type["mpeg"] = "video/mpeg";
    content_type["mpg"] = "video/mpeg";
    content_type["mov"] = "video/quicktime";
    content_type["flv"] = "video/x-flv";
    content_type["avi"] = "video/x-msvideo";
    content_type["wmv"] = "video/x-ms-wmv";
    content_type["asx"] = "video/x-ms-asf";
    content_type["asf"] = "video/x-ms-asf";
    content_type["mng"] = "video/x-mng";
    ///////////////////////////////////////////
    status_message[100] = "Continue";
    status_message[101] = "Switching Protocols";
    status_message[102] = "Processing";

    status_message[200] = "OK";
    status_message[201] = "Created";
    status_message[202] = "Accepted";
    status_message[203] = "Non-Authoritative Information";
    status_message[204] = "No Content";
    status_message[205] = "Reset Content";
    status_message[206] = "Partial Content";
    status_message[207] = "Multi-Status";
    status_message[208] = "Already Reported";
    status_message[226] = "IM Used";

    status_message[300] = "Multiple Choices";
    status_message[301] = "Moved Permanently";
    status_message[302] = "Found";
    status_message[303] = "See Other";
    status_message[304] = "Not Modified";
    status_message[305] = "Use Proxy";
    status_message[307] = "Temporary Redirect";
    status_message[308] = "Permanent Redirect";

    status_message[400] = "Bad Request";
    status_message[401] = "Unauthorized";
    status_message[402] = "Payment Required";
    status_message[403] = "Forbidden";
    status_message[404] = "Not Found";
    status_message[405] = "Method Not Allowed";
    status_message[406] = "Not Acceptable";
    status_message[407] = "Proxy Authentication Required";
    status_message[408] = "Request Timeout";
    status_message[409] = "Conflict";
    status_message[410] = "Gone";
    status_message[411] = "Length Required";
    status_message[412] = "Precondition Failed";
    status_message[413] = "Payload Too Large";
    status_message[414] = "URI Too Long";
    status_message[415] = "Unsupported Media Type";
    status_message[416] = "Range Not Satisfiable";
    status_message[417] = "Expectation Failed";
    status_message[418] = "I'm a teapot";
    status_message[421] = "Misdirected Request";
    status_message[422] = "Unprocessable Entity";
    status_message[423] = "Locked";
    status_message[424] = "Failed Dependency";
    status_message[426] = "Upgrade Required";
    status_message[428] = "Precondition Required";
    status_message[429] = "Too Many Requests";
    status_message[431] = "Request Header Fields Too Large";
    status_message[451] = "Unavailable For Legal Reasons";

    status_message[500] = "Internal Server Error";
    status_message[501] = "Not Implemented";
    status_message[502] = "Bad Gateway";
    status_message[503] = "Service Unavailable";
    status_message[504] = "Gateway Timeout";
    status_message[505] = "HTTP Version Not Supported";
    status_message[506] = "Variant Also Negotiates";
    status_message[507] = "Insufficient Storage";
    status_message[508] = "Loop Detected";
    status_message[510] = "Not Extended";
    status_message[511] = "Network Authentication Required";
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
    if (content_type.find(extension) != content_type.end())
        return content_type[extension];
    else
        return "application/octet-stream";
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
    if (status_message.find(status_code) != status_message.end())
        headersField[".status_message"] = status_message[status_code];
    else
        headersField[".status_message"] = "Internal Server Error";
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
    if (headersField["Content-Type"].find("video") != std::string::npos || headersField["Content-Type"].find("audio") != std::string::npos)
    {
        headersField["Accept-Ranges"] = "bytes";
    }
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
    if (status_message.find(status_code) != status_message.end())
        return status_message[status_code];
    else
        return "Internal Server Error";
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
