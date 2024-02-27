/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 21:29:02 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 23:03:04 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

Config Config::instance;

Config& Config::getInstance() {
    if (!instance.isInitialized) {
        throw std::runtime_error("Config not initialized");
    }
    return instance;
}

Config& Config::init(const std::string& filePath) {
    if (instance.isInitialized) {
        throw std::runtime_error("Config already initialized");
    }
    Parser::init(filePath);
    instance.initRules();
    instance.initMime();
    instance.initStatusCodes();
    instance.isInitialized = true;
    instance.read();
    return instance;
}

void Config::destroy() {
    Parser::destroy();
}

Config::Config() : isInitialized(false) {
}

const string& Config::getMimeType(const string& ext) {
    map<string, string>::iterator it = mime_types.find(ext);
    if (it != mime_types.end())
        return (it->second);
    return (mime_types["default"]);
}

const string& Config::getStatusCode(int code) {
    map<int, string>::iterator it = status_codes.find(code);
    if (it != status_codes.end())
        return (it->second);
    return (status_codes[500]);
}

void Config::initMime() {
    mime_types["html"] = "text/html";
    mime_types["htm"] = "text/html";
    mime_types["shtml"] = "text/html";
    mime_types["css"] = "text/css";
    mime_types["xml"] = "text/xml";
    mime_types["rss"] = "text/xml";
    mime_types["gif"] = "image/gif";
    mime_types["jpeg"] = "image/jpeg";
    mime_types["jpg"] = "image/jpeg";
    mime_types["js"] = "application/x-javascript";
    mime_types["txt"] = "text/plain";
    mime_types["htc"] = "text/x-component";
    mime_types["mml"] = "text/mathml";
    mime_types["png"] = "image/png";
    mime_types["ico"] = "image/x-icon";
    mime_types["jng"] = "image/x-jng";
    mime_types["bmp"] = "image/bmp";
    mime_types["svg"] = "image/svg+xml";
    mime_types["svgz"] = "image/svg+xml";
    mime_types["tif"] = "image/tiff";
    mime_types["tiff"] = "image/tiff";
    mime_types["wbmp"] = "image/vnd.wap.wbmp";
    mime_types["avif"] = "image/avif";
    mime_types["jar"] = "application/java-archive";
    mime_types["war"] = "application/java-archive";
    mime_types["ear"] = "application/java-archive";
    mime_types["hqx"] = "application/mac-binhex40";
    mime_types["pdf"] = "application/pdf";
    mime_types["cco"] = "application/x-cocoa";
    mime_types["jardiff"] = "application/x-java-archive-diff";
    mime_types["jnlp"] = "application/x-java-jnlp-file";
    mime_types["run"] = "application/x-makeself";
    mime_types["pl"] = "application/x-perl";
    mime_types["pm"] = "application/x-perl";
    mime_types["prc"] = "application/x-pilot";
    mime_types["pdb"] = "application/x-pilot";
    mime_types["rar"] = "application/x-rar-compressed";
    mime_types["rpm"] = "application/x-redhat-package-manager";
    mime_types["sea"] = "application/x-sea";
    mime_types["swf"] = "application/x-shockwave-flash";
    mime_types["sit"] = "application/x-stuffit";
    mime_types["tcl"] = "application/x-tcl";
    mime_types["tk"] = "application/x-tcl";
    mime_types["der"] = "application/x-x509-ca-cert";
    mime_types["pem"] = "application/x-x509-ca-cert";
    mime_types["crt"] = "application/x-x509-ca-cert";
    mime_types["xpi"] = "application/x-xpinstall";
    mime_types["zip"] = "application/zip";
    mime_types["deb"] = "application/octet-stream";
    mime_types["bin"] = "application/octet-stream";
    mime_types["exe"] = "application/octet-stream";
    mime_types["dll"] = "application/octet-stream";
    mime_types["dmg"] = "application/octet-stream";
    mime_types["eot"] = "application/octet-stream";
    mime_types["iso"] = "application/octet-stream";
    mime_types["img"] = "application/octet-stream";
    mime_types["msi"] = "application/octet-stream";
    mime_types["msp"] = "application/octet-stream";
    mime_types["msm"] = "application/octet-stream";
    mime_types["mp3"] = "audio/mpeg";
    mime_types["mp4"] = "video/mp4";
    mime_types["ra"] = "audio/x-realaudio";
    mime_types["mpeg"] = "video/mpeg";
    mime_types["mpg"] = "video/mpeg";
    mime_types["mov"] = "video/quicktime";
    mime_types["flv"] = "video/x-flv";
    mime_types["avi"] = "video/x-msvideo";
    mime_types["wmv"] = "video/x-ms-wmv";
    mime_types["asx"] = "video/x-ms-asf";
    mime_types["asf"] = "video/x-ms-asf";
    mime_types["mng"] = "video/x-mng";
    mime_types["default"] = "application/octet-stream";
    ///////////////////////////////////////////
    
}

void Config::initStatusCodes() {
    status_codes[100] = "Continue";
    status_codes[101] = "Switching Protocols";
    status_codes[102] = "Processing";

    status_codes[200] = "OK";
    status_codes[201] = "Created";
    status_codes[202] = "Accepted";
    status_codes[203] = "Non-Authoritative Information";
    status_codes[204] = "No Content";
    status_codes[205] = "Reset Content";
    status_codes[206] = "Partial Cmime_typesontent";
    status_codes[207] = "Multi-Status";
    status_codes[208] = "Already Reported";
    status_codes[226] = "IM Used";

    status_codes[300] = "Multiple Choices";
    status_codes[301] = "Moved Permanently";
    status_codes[302] = "Found";
    status_codes[303] = "See Other";
    status_codes[304] = "Not Modified";
    status_codes[305] = "Use Proxy";
    status_codes[307] = "Temporary Redirect";
    status_codes[308] = "Permanent Redirect";

    status_codes[400] = "Bad Request";
    status_codes[401] = "Unauthorized";
    status_codes[402] = "Payment Required";
    status_codes[403] = "Forbidden";
    status_codes[404] = "Not Found";
    status_codes[405] = "Method Not Allowed";
    status_codes[406] = "Not Acceptable";
    status_codes[407] = "Proxy Authentication Required";
    status_codes[408] = "Request Timeout";
    status_codes[409] = "Conflict";
    status_codes[410] = "Gone";
    status_codes[411] = "Length Required";
    status_codes[412] = "Precondition Failed";
    status_codes[413] = "Payload Too Large";
    status_codes[414] = "URI Too Long";
    status_codes[415] = "Unsupported Media Type";
    status_codes[416] = "Range Not Satisfiable";
    status_codes[417] = "Expectation Failed";
    status_codes[418] = "I'm a teapot";
    status_codes[421] = "Misdirected Request";
    status_codes[422] = "Unprocessable Entity";
    status_codes[423] = "Locked";
    status_codes[424] = "Failed Dependency";
    status_codes[426] = "Upgrade Required";
    status_codes[428] = "Precondition Required";
    status_codes[429] = "Too Many Requests";
    status_codes[431] = "Request Header Fields Too Large";
    status_codes[451] = "Unavailable For Legal Reasons";

    status_codes[500] = "Internal Server Error";
    status_codes[501] = "Not Implemented";
    status_codes[502] = "Bad Gateway";
    status_codes[503] = "Service Unavailable";
    status_codes[504] = "Gateway Timeout";
    status_codes[505] = "HTTP Version Not Supported";
    status_codes[506] = "Variant Also Negotiates";
    status_codes[507] = "Insufficient Storage";
    status_codes[508] = "Loop Detected";
    status_codes[510] = "Not Extended";
    status_codes[511] = "Network Authentication Required";
}

void Config::initRules() {
    Location::initValidationMap();
    setAlloedDirective();
    defaultConfig.insert(std::make_pair("root", "www"));
    defaultConfig.insert(std::make_pair("index", "index.html"));
    defaultConfig.insert(std::make_pair("autoindex", "on"));
    defaultConfig.insert(std::make_pair("client_max_body_size", "1m"));
    defaultConfig.insert(std::make_pair("methods", "GET"));
    defaultConfig.insert(std::make_pair("upload", "off"));
    defaultConfig.insert(std::make_pair("upload_path", "/uploads"));
    defaultConfig.insert(std::make_pair("cgi", "off"));
    defaultConfig.insert(std::make_pair("cgi_timeout", "30"));
    defaultConfig.insert(std::make_pair("client_timeout", "12"));
    defaultConfig.insert(std::make_pair("allowed_cgi", "php /usr/bin/php-cgi ; py /usr/bin/python3"));
}

Config::~Config() {
    Parser::destroy();
}

map<string, int> Config::directive;
map<string, string> Config::mime_types;
map<int, string> Config::status_codes;

void Config::setAlloedDirective() {
    if (!directive.empty()) {
        return ;
    }
    directive.insert(std::make_pair("server", 0));
}

void Config::read() {
    // try {
        readMainContext();
    // }
    // catch (std::exception& e) {
        // std::cerr << e.what() << std::endl;
    // }
    Parser::destroy();
}

void Config::readMainContext() {
    std::string token;
    while (true) {
        token = Parser::getTok();
        if (token.empty())
            break;
        if (token == ";")
            continue;
        set(token);
    }
}

void Config::setServer() {
        servers.push_back(Server());
        servers.back().set();
}   

void Config::set(const string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        throw ConfigException::DIRECT_NOT_VALID(token);
    }
    if (token == "server") {
        setServer();
        return ;
    }
}

Location& Config::getLocation(const string& uri) {
    string server_name = uri.substr(0, uri.find("/"));
    string location = uri.substr(uri.find("/"));
    Server* serv = NULL;
    for (deque<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
        for (size_t i = 0; i < it->server_name.size(); i++) {
            if (it->server_name[i] == server_name) {
                serv = &(*it);
                break;
            }
        }
        if (server_name == it->listenIp) {
            serv = &(*it);
            break;
        }
    }
    if (!serv && !servers.empty()) {
        serv = &servers[0];
    } else if (!serv) {
       // -- cout << "server not found" << endl;
        throw std::runtime_error("Error: server not found");
    }
    return (serv->getRootLocation().getLocation(location));
}

deque<Server>& Config::getServers() {
    return (servers);
}

const string Config::getDefault(const string& key) {
    if (!instance.isInitialized) {
        throw std::runtime_error("Config not initialized");
    }
    KeyVal::iterator it = instance.defaultConfig.find(key);
    if (it != instance.defaultConfig.end())
        return (it->second);
    return "";
}

void Config::print () {
   // -- cout << "*************printing**************" << endl;
   // -- cout << "server size is " << servers.size() << endl;
    for (size_t i = 0; i < servers.size(); i++) {
       // -- cout << "server " << i << " server_name is ";
        for (size_t j = 0; j < servers[i].server_name.size(); j++) {
           // -- cout << servers[i].server_name[j] << " ";
        }
       // -- cout << endl;
       // -- cout << "server " << i << " listenIp is " << servers[i].listenIp << endl;
       // -- cout << "server " << i << " listenPort is " << servers[i].listenPort << endl;
       // -- cout << "server " << i << " error_page is " << servers[i].error_page << endl;
        //// -- cout << "server " << i << " location size is " << servers[i].locations.size() << endl;
        servers[i].getRootLocation().print(0);
   // -- cout << "*************printing done**************" << endl;
    }
}
