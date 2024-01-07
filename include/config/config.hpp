/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/07 23:34:42 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "configFlag.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
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



















class location;
class server;

class Parser {
public:
    Parser(const std::string& filePath);
    ~Parser();
    bool findBracket(const std::string& bracket);
    bool is_space(const char c);
    bool is_delimitter(const char c);
    bool isEndingLine();
    std::string getToken();
    int getLineNum();
    std::vector<configScope>& getScopes();

private:
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    std::vector<configScope> scopes;
};

class Config {
public:
    Config(const string& filePath);
    ~Config();
    void read();
    void print();
    void set(const string& token);
private:
    void readMainContext();
    Parser p;
    vector<server> servers;
    vector<location> locations;
    static map<string, int> directive;
};

struct server {
    vector<string>  server_name;
    string  listenIp;
    int     listenPort;
    string  error_page;
    server(Parser& p);
    ~server();
    void setServerInfo(const string& token);
    void parseListen();
    void parseServerName();
    const string& validateIp(const string& ip);
    int validatePort(const string& port);
    void checkServerInfo();
    map<string, location*> location; // uri, location
private:
    static map<string, int> directive;
    map<string, string> info;
    Parser& p;
};

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
    location(Parser& p, server& serv, const string& uri);
    ~location();
    void setLocationInfo(const string& token);
    void checkLocationInfo();
    string& getUri() const;
private:
    string uri;
    map<string, string> info;
    static map<string, int> directive;
    server &serv;
    Parser &p;
};


#endif // CONFIG_HPP