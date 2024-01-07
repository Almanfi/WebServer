/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:52:48 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/06 21:16:02 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

// #include <iostream>
// #include <fstream>
// #include <string>
// #include <map>
// #include <vector>

// using std::cin;
// using std::cout;
// using std::endl;
// using std::string;
// using std::map;
// using std::vector;

// typedef enum e_configScope {
//     MAIN,
//     SERVER,
//     LOCATION
// } configScope;



// class ConfigParser;
// class serverContext;
// class locationContext;


locationContext::locationContext(ConfigParser &parent, serverContext &server) : parent(parent), server(server), scopes(parent.getScopes()) {
    uri = "/";
    root = "";
    index = "";
    autoindex = false;
    client_max_body_size = 0;
    cgi = "";
    upload_store = "";
    upload_pass = "";
    upload_pass_args = "";
    methods = "";
    return_val = "";
    auth_basic = "";
    auth_basic_user_file = "";
}

locationContext::~locationContext() {}

void locationContext::setUri() {
    server.error_page = "";
    string token;
    token = parent.getToken();
    if (token.empty() && token[0] != '/') {
        std::cerr << "Error: not a valid URI '" << token << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    uri = token;
}

void locationContext::parseLocationContext() {
    if (scopes.back() != LOCATION && scopes.back() != SERVER) {
        std::cerr << "Error: (in parse location) Invalid context 'location' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    setUri();
    if (parent.findBracket("{"))
        scopes.push_back(LOCATION);
    else {
        std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    string token;
    while (true) {
        token = parent.getToken();
        if (token.empty())
            break;
        else if (token == "}")
            break;
        setLocationData(token);
    }
    if (parent.isEndingLine() == false && parent.getToken() != ";") {
        std::cerr << "Error:(in parseLocationContext) Invalid token '" << parent.getToken() << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    if (scopes.back() != LOCATION) {
        std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    scopes.pop_back();
}

void locationContext::setLocationData(const string& token) {
    cout << "setLocationData" << endl;
    if (token == "root") {
        root = parent.getToken();
    }
    else if (token == "index") {
        index = parent.getToken();
    }
    else if (token == "autoindex") {
        string token = parent.getToken();
        if (token == "on") {
            autoindex = true;
        }
        else if (token == "off") {
            autoindex = false;
        }
        else {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
    }
    else if (token == "client_max_body_size") {
        string token = parent.getToken();
        client_max_body_size = std::strtol(token.c_str(), NULL, 10);
    }
    else if (token == "cgi") {
        cgi = parent.getToken();
    }
    else if (token == "upload_store") {
        upload_store = parent.getToken();
    }
    else if (token == "upload_pass") {
        upload_pass = parent.getToken();
    }
    else if (token == "upload_pass_args") {
        upload_pass_args = parent.getToken();
    }
    else if (token == "methods") {
        methods = parent.getToken();
    }
    else if (token == "return") {
        return_val = parent.getToken();
    }
    else if (token == "auth_basic") {
        auth_basic = parent.getToken();
    }
    else if (token == "auth_basic_user_file") {
        auth_basic_user_file = parent.getToken();
    }
    else {
        std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    if (parent.getToken() != ";") {
        std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
}

serverContext::serverContext(ConfigParser &parent) : parent(parent) {
    listenPort = 80;
    listenIp = "";
    error_page = "";
    location.insert(std::pair<string, locationContext>("/", locationContext(parent, *this)));
}

serverContext::~serverContext() {}

void serverContext::setServerName() {
    string token;
    while (true) {
        token = parent.getToken();
        if (token == ";") {
            break;
        }
        else {
            server_name.push_back(token);
        }
    }
    if (server_name.empty()) {
        server_name.push_back("");
    }
}

void serverContext::setLiten() {
    string token;
    token = parent.getToken();
    if (token.find(":") != string::npos) {
        listenIp = token.substr(0, token.find(":"));
        listenPort = std::strtol(token.substr(token.find(":") + 1).c_str(), NULL, 10);
    }
    else {
        listenIp = "";
        listenPort = std::strtol(token.c_str(), NULL, 10);
    }
    token = parent.getToken();
    if (token != ";") {
        std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
    // validate port number and ip address
    // if (listenPort < 0 || listenPort > 65535) {
    //     std::cerr << "Error: Invalid port number '" << token << "' at line " << parent.getLineNum() << std::endl;
    //     throw std::exception();
    // }
}

void serverContext::setErrorPage() {
    string token;
    token = parent.getToken();
    if (token != ";") {
        error_page = token;
        token = parent.getToken();
        if (token != ";") {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
    }
    else {
        std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
        throw std::exception();
    }
}

void serverContext::createNewLocation() {
    locationContext newLocation(parent, *this);
    newLocation.parseLocationContext();
    location.insert(std::pair<string, locationContext>(newLocation.uri, newLocation));
}

void serverContext::setLocation(const string& token) {
    cout << "setLocation" << endl;
    if (token == "location") {
        cout << "creating new location" << endl;
        createNewLocation();
    }
    else {
        cout << "setting location data " << token << endl;
        locationContext& loc = location.find("/")->second;
        loc.setLocationData(token);
    }
}

ConfigParser::ConfigParser(const std::string& filePath) : filePath(filePath), lineNumber(0), linePos(0) { 
}

bool ConfigParser::parse() {
    configFile.open(filePath.c_str(), std::ios::in);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << filePath << std::endl;
        return false;
    }
    try {
        parseMainContext();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        configFile.close();
        return false;
    } 
    configFile.close();
    return true;
}

void ConfigParser::parseMainContext() {
    scopes.push_back(MAIN);
    std::string token;
    while (true) {
        token = getToken();
        if (token.empty())
            break;
        if (token == "server") {
            parseServerContext();
        }
        else {
            std::cerr << "Error: Invalid token '" << token << "' at line " << lineNumber << std::endl;
            throw std::exception();
        }
    }
    if (scopes.back() != MAIN) {
        std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
        throw std::exception();
    }
}

void ConfigParser::parseServerContext() {
    cout << "parseServerContext" << endl;
    if (scopes.back() != MAIN) {
        std::cerr << "Error: Invalid context 'server' at line " << lineNumber << std::endl;
        throw std::exception();
    }
    if (findBracket("{"))
        scopes.push_back(SERVER);
    else {
        std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
        throw std::exception();
    }
    server.push_back(serverContext(*this));
    cout << "adding new server " << server.size() << endl;
    std::string token;
    while (true) {
        token = getToken();
        if (token.empty())
            break;
        if (token == "server_name") {
            server.back().setServerName();
        }
        else if (token == "listen") {
            server.back().setLiten();
        }
        else if (token == "}") {
            break;
        }
        else {
            server.back().setLocation(token);
        }
    }
    if (isEndingLine() == false) {
        std::cerr << "Error: Invalid token '" << getToken() << "' at line " << lineNumber << std::endl;
        throw std::exception();
    }
    if (scopes.back() != SERVER) {
        std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
        throw std::exception();
    }
    scopes.pop_back();
    cout << "*************serverparsing done**************" << endl;
    cout << "on this serever rout is " << server.back().location.find("/")->second.root << endl;
}

bool ConfigParser::findBracket(const std::string& bracket) {
    std::string token;
    while (true) {
        token = getToken();
        if (token.empty())
            break;
        if (token == bracket) {
            return true;
        }
        else {
            std::cerr << "Error: Invalid token (need an opening bracket) '" << token << "' at line " << lineNumber << std::endl;
            throw std::exception();
        }
    }
    return false;
}

bool ConfigParser::is_space(const char c) {
    if (c == ' ' || c == '\t')
        return true;
    return false;
}

bool ConfigParser::is_delimitter(const char c) {
    if (c == '{' || c == '}' || c == ';')
        return true;
    return false;
}

bool ConfigParser::isEndingLine() {
    while (is_space(line[linePos]))
        linePos++;
    if (line[linePos] == '#' || linePos >= line.length()) {
        if (!std::getline(configFile, line))
            line = "";
        lineNumber++;
        cout << "line " << lineNumber << ": " << line << endl;
        linePos = 0;
        return true;
    }
    return false;
}

std::string ConfigParser::getToken() {
    size_t& i = linePos;
    if (i >= line.length()) {
        if (!std::getline(configFile, line)) {
            return string("");
        }
        lineNumber++;
        cout << "line " << lineNumber << ": " << line << endl;
        i = 0;
    }
    while (is_space(line[i]))
        i++;
    if (line[i] == '#' || i >= line.length()) {
        i = line.length();
        std::cout << "Comment : skip line!" << std::endl;
        return getToken();
    }
    if (is_delimitter(line[i])) {
        i++;
        std::string delimitter = line.substr(i - 1, 1);
        if (delimitter == ";") {
            while (is_space(line[i]))
                i++;
            if (line[i] == '#' || i >= line.length()) {
                i = line.length();
                std::cout << "Comment : skip line after ';'!" << std::endl;
            }
            else {
                std::cerr << "Error: found token after ';' " << " at line " << lineNumber << std::endl;
                throw std::exception();
            }
        }
        return delimitter;
    }
    size_t start = i;
    while (!is_space(line[i]) && !is_delimitter(line[i]) && line[i] != '\0')
        i++;
    return line.substr(start, i - start);
}

int ConfigParser::getLineNum() {
    return lineNumber;
}

std::vector<configScope>& ConfigParser::getScopes() {
    return scopes;
}

std::vector<serverContext> ConfigParser::getServer() {
    return server;
}

// const std::vector<string> ConfigParser::mainContextWords = {"server", "location"};
// const std::vector<string> ConfigParser::serverContextWords = {"listen", "server_name", "error_page", "location", "root", "index", "autoindex", "client_max_body_size", "cgi", "upload_store", "upload_pass", "upload_pass_args", "methods", "return", "auth_basic", "auth_basic_user_file"};
// const std::vector<string> ConfigParser::locationContextWords = {"root", "index", "autoindex", "client_max_body_size", "cgi", "upload_store", "upload_pass", "upload_pass_args", "methods", "return", "auth_basic", "auth_basic_user_file"};

int main() {
    ConfigParser parser("./config/default.config");
    if (parser.parse()) {
        std::cout << "parsing done"<< std::endl;
    }
    vector<serverContext> server = parser.getServer();
    for (vector<serverContext>::iterator it = server.begin(); it != server.end(); it++) {
        cout << "server name: ";
        for (vector<string>::iterator it2 = it->server_name.begin(); it2 != it->server_name.end(); it2++) {
            cout << *it2 << " ";
        }
        cout << endl;
        cout << "listen ip: " << it->listenIp << endl;
        cout << "listen port: " << it->listenPort << endl;
        cout << "error page: " << it->error_page << endl;
        cout << "**location: " << it->location.size() << endl;
        for (map<string, locationContext>::iterator it2 = it->location.begin(); it2 != it->location.end(); it2++) {
            cout << "*URI: " << it2->second.uri << endl;
            cout << " root: " << it2->second.root << endl;
            cout << " index: " << it2->second.index << endl;
            cout << " autoindex: " << it2->second.autoindex << endl;
            cout << " client_max_body_size: " << it2->second.client_max_body_size << endl;
            cout << " cgi: " << it2->second.cgi << endl;
            cout << " upload_store: " << it2->second.upload_store << endl;
            cout << " upload_pass: " << it2->second.upload_pass << endl;
            cout << " upload_pass_args: " << it2->second.upload_pass_args << endl;
            cout << " methods: " << it2->second.methods << endl;
            cout << " return: " << it2->second.return_val << endl;
            cout << " auth_basic: " << it2->second.auth_basic << endl;
            cout << " auth_basic_user_file: " << it2->second.auth_basic_user_file << endl;
        }
    }
    // locationContext& loc = parser.getServer()[0].location.find("/")->second;
    // cout << loc.root << endl;
    // cout << parser.getServer().size() << endl;
    // cout << parser.getServer()[0].listenPort << endl;
    // cout << parser.getServer()[1].listenPort << endl;

    return 0;
}
