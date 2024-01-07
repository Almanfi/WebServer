/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/07 17:46:23 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "configFlag.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;


typedef enum e_configScope {
    MAIN,
    SERVER,
    LOCATION
} configScope;

class ConfigParser;
class serverContext;
class locationContext;

class locationContext {
public:
    locationContext(ConfigParser &parent, serverContext &server);
    ~locationContext();
    void setUri();
    void parseLocationContext();
    void setLocationData(const string& token);

    std::string uri;
    std::string root;
    std::string index;
    bool autoindex;
    int client_max_body_size;
    std::string cgi;
    std::string upload_store;
    std::string upload_pass;
    std::string upload_pass_args;
    std::string methods;
    std::string return_val;
    std::string auth_basic;
    std::string auth_basic_user_file;
private:
    ConfigParser &parent;
    serverContext &server;
    std::vector<configScope>& scopes;
};

class serverContext {
public:
    serverContext(ConfigParser &parent);
    ~serverContext();
    
    void setServerName();
    void setLiten();
    void setErrorPage();
    void createNewLocation();
    void setLocation(const string& token);
    
    vector<string> server_name;
    string  listenIp;
    int     listenPort;
    std::string error_page;
    map<string, locationContext> location;

private:
    ConfigParser &parent;
};

class ConfigParser {
public:
    ConfigParser(const std::string& filePath);
    bool parse();
    void parseMainContext();
    void parseServerContext();
    bool findBracket(const std::string& bracket);
    bool is_space(const char c);
    bool is_delimitter(const char c);
    bool isEndingLine();
    std::string getToken();
    int getLineNum();
    std::vector<configScope>& getScopes();
    std::vector<serverContext> getServer();

private:
    std::string filePath;
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    std::vector<configScope> scopes;
    std::map<std::string, std::string> configData;
    std::map<std::string, std::string> mainContextData;
    std::vector<serverContext> server;
};

class Config {
    vector<server> servers;
    vector<location> locations;
};

struct server {
    vector<string>  server_name;
    string  listenIp;
    int     listenPort;
    string  error_page;
    void setServerInfo(const string& token);
    void parseListen();
    void checkServerInfo();
    server(ConfigParser& p);
private:
    static map<string, int> directive;
    map<string, string> info;
    // map<string, location> location; // uri, location
    ConfigParser& p;
};

server::server(ConfigParser& p) : p(p) {
}

server::~server() {
}

map<string, int> server::directive = {
    std::make_pair("server_name", 0),
    std::make_pair("listen", 1),
    std::make_pair("error_page", 1)
};

void server::setServerInfo(const string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    if (info.find(token) != info.end()) {
        throw locExp::DIRECT_ALREADY_SET();
    }
    int count = 0;
    string value = "";
    string newToken = "";
    while (true) {
        newToken = p.getToken();
        if (newToken == ";") {
            break;
        }
        if (count > 1) {
            value += " ";
        }
        value += newToken;
        count++;
    }
    if (it->second && it->second != count) {
        throw locExp::TOO_MANY_ARGS();
    }
    info[token] = value;
}

void server::parseListen() {
    string listen = info["listen"];
    size_t colon = listen.find(":");
    if (colon == string::npos) {
        listenIp = "";
        listenPort = 80;
        return ;
    }
    string ipStr = listen.substr(0, colon);
    string portStr = listen.substr(colon + 1);
    listenPort =  std::strtol(portStr.c_str(), NULL, 10);
    if (listenPort < 0 || listenPort > 65535) {
        throw locExp::NOT_VALID_PORT();
    }

}

void server::checkServerInfo() {
    if (info.find("listen") == info.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    listenIp = info["listen"].substr(0, info["listen"].find(":"));
    listenPort = atoi(info["listen"].substr(info["listen"].find(":") + 1).c_str());
    if (info.find("server_name") == info.end()) {
        if (listenIp.empty())
            server_name.push_back("127.0.0.1");
        else
            server_name.push_back(listenIp);
    }
    if (info.find("error_page") == info.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
}

class locExp {
public:
    class DIRECT_NOT_VALID: public std::exception {
        virtual const char* what() const throw() {
            return "Error: directive not valid";
        }
    };
    class TOO_MANY_ARGS: public std::exception {
        virtual const char* what() const throw() {
            return "Error: too many arguments";
        }
    };
    class DIRECT_ALREADY_SET: public std::exception {
        virtual const char* what() const throw() {
            return "Error: directive already set";
        }
    };
    class NOT_VALID_PORT: public std::exception {
        virtual const char* what() const throw() {
            return "Error: not valid port";
        }
    };
};

class location : public server {
public:
    location(ConfigParser& p, server& serv);
    ~location();
    void setServerInfo(const string& token);
    void setLocationInfo(const string& token);

private:
    map<string, string> info;
    static map<string, int> directive;
    server &serv;
    ConfigParser &p;
};

location::location(ConfigParser& p, server& serv) : p(p), serv(serv) {
}

location::~location() {
}

map<string, int> location::directive = {
    std::make_pair("root", 1),
    std::make_pair("index", 0),
    std::make_pair("autoindex", 1),
    std::make_pair("client_max_body_size", 1),
    std::make_pair("cgi", 0),
    std::make_pair("upload_store", 1),
    std::make_pair("upload_pass", 1),
    std::make_pair("upload_pass_args", 1),
    std::make_pair("methods", 1),
    std::make_pair("return", 1),
    std::make_pair("auth_basic", 1),
    std::make_pair("auth_basic_user_file", 1),
    std::make_pair("access_log", 1)
};

void location::setLocationInfo(const string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    if (info.find(token) != info.end()) {
        throw locExp::DIRECT_ALREADY_SET();
    }
    int count = 0;
    string value = "";
    string newToken = "";
    while (true) {
        newToken = p.getToken();
        if (newToken == ";") {
            break;
        }
        if (count > 1) {
            value += " ";
        }
        value += newToken;
        count++;
    }
    if (it->second && it->second != count) {
        throw locExp::TOO_MANY_ARGS();
    }
    info[token] = value;
}



#endif // CONFIG_HPP