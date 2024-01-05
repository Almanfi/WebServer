/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/05 23:40:13 by maboulkh         ###   ########.fr       */
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



#endif // CONFIG_HPP