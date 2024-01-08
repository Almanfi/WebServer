/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cnofig2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 21:29:02 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/08 15:59:33 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

map<string, int> server::directive = {
    std::make_pair("server_name", 0),
    std::make_pair("listen", 1),
    std::make_pair("error_page", 1)
};

server::server(Parser& p) : p(p) {
}

// server::server() : p(*(new Parser("default.config"))) {
// }

server::~server() {
}

void server::setMainLocation(string& token) {
    map<string, location*>::iterator it = locations.find("/");
    if (it == locations.end()) {
        locations.insert(std::make_pair("/", new location(p, *this, "/")));
    }
    location& loc = *(locations.find("/")->second);
    loc.setLocationInfo(token);
}

void server::setServerInfo(string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        setMainLocation(token);
    }
    // if (token == "location") {
    //     // location loc(p, *this, p.getToken());
    //     return ;
    // }
    if (info.find(token) != info.end()) {
        throw locExp::DIRECT_ALREADY_SET();
    }
    int count = 0;
    string value = "";
    string newToken = "";
    while (true) {
        newToken = p.getToken();
        if (newToken == ";" || newToken.empty()) {
            break;
        }
        if (count > 1) {
            value += " ";
        }
        value += newToken;
        count++;
        if (it->second && it->second < count) {
            throw locExp::TOO_MANY_ARGS();
        }
    }
    // if (newToken != ";") { // unnecessary!
    //     throw locExp::DIRECT_NOT_VALID();
    // }
    info[token] = value;
    token = newToken;
}

const string& server::validateIp(const string& ip) {
    std::stringstream ss(ip);
    int num;
    char dot;

    ss >> num;
    if (ss.fail() || num < 0 || num > 255)
        throw std::runtime_error("Invalid IP address");
    cout << num << endl;
    int i = 0;
    while (i++ < 3 && ss >> dot >> num) {
        if (ss.fail() || dot != '.')
            throw std::runtime_error("Invalid IP address");
        if (num < 0 || num > 255)
            throw std::runtime_error("Invalid IP address");
        cout << num << endl;
    }
    if (!ss.eof() || i != 4)
        throw std::runtime_error("Invalid IP address");
    return (ip);
}
int server::validatePort(const string& portStr) {
    if (portStr[0] == '+' || portStr[0] == '-'
        || (portStr[0] == '0' && portStr.size() > 1))
        throw locExp::NOT_VALID_PORT();
    std::stringstream ss(portStr);
    int port;
    ss >> port;
    if (ss.fail() || !ss.eof() || port < 0 || port > 65535)
        throw locExp::NOT_VALID_PORT();
    return (port);
}

void server::parseListen() {
    string listen = info["listen"];
    size_t colon = listen.find(":");
    if (colon == string::npos) {
        listenIp = "";
        listenPort = validatePort(listen);
        return ;
    }
    string ipStr = listen.substr(0, colon);
    listenIp = validateIp(ipStr);
    string portStr = listen.substr(colon + 1);
    listenPort = validatePort(portStr);
}

void server::parseServerName() {
    if (info.find("server_name") == info.end()) {
        if (listenIp.empty())
            server_name.push_back("127.0.0.1");
        else
            server_name.push_back(listenIp);
    }
    else {
        string serverName = info["server_name"];
        size_t space = serverName.find(" ");
        while (true) {
            if (space == string::npos) {
                server_name.push_back(serverName);
                break;
            }
            server_name.push_back(serverName.substr(0, space));
            serverName = serverName.substr(space + 1);
            space = serverName.find(" ");
        }
    }
}

void server::checkServerInfo() {
    if (info.find("listen") == info.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    parseListen();
    parseServerName();
}

location::location(Parser& p, server& serv, const string& uri) : p(p), serv(serv), uri(uri) {
    // cout << "location constructor for " << endl;
    // cout << "uri is " << location::uri << endl;
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

void location::set() {
    vector<configScope>& scope = p.getScopes();
    scope.push_back(LOCATION);
    // string theToken = p.getToken();
    // cout << "theToken is " << theToken << endl;
    if (p.getToken() != "{") {
        throw std::runtime_error("Error: Missing opening bracket '{' at line " + std::to_string(p.getLineNum()));
    }
    std::string token;
    while (true) {
        token = p.getToken();
        if (token == "}" || token.empty()) {
            break;
        }
        setLocationInfo(token);
    }
    if (scope.back() != LOCATION || token.empty()) {
        throw std::runtime_error("Error: Missing closing bracket '}' at line " + std::to_string(p.getLineNum()));
    }
    scope.pop_back();
    checkLocationInfo();
}

string& location::getUri() const {
    // cout << "getUri is " << uri << endl;
    return (const_cast<string&>(uri));
}

void location::setLocationInfo(string& token) {
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
        if (newToken == ";" || newToken.empty()) {
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
    token = newToken;
}

void location::checkLocationInfo() {
}









Config::Config(const std::string& filePath) : p(filePath) {
}

Config::~Config() {
}

map<string, int> Config::directive = {
    std::make_pair("server", 0)
};

void Config::read() {
    try {
        readMainContext();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Config::readMainContext() {
    vector<configScope>& scopes = p.getScopes();
    scopes.push_back(MAIN);
    std::string token;
    while (true) {
        token = p.getToken();
        if (token.empty())
            break;
            set(token);
    }
    if (scopes.back() != MAIN) {
        throw std::runtime_error("Error: Missing closing bracket '}' at line " + std::to_string(p.getLineNum()));
    }
    scopes.pop_back();
}

void Config::set(const string& token) {
    bool hasLocation = false;
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    // if (info.find(token) != info.end()) {
    //     throw locExp::DIRECT_ALREADY_SET();
    // }
    if (token == "server") {
        vector<configScope>& scopes = p.getScopes();
        scopes.push_back(SERVER);
        string newToken = p.getToken();
        if (newToken != "{") {
            throw std::runtime_error("Error: Missing opening bracket '{' at line " + std::to_string(p.getLineNum()));
        }
        servers.push_back(server(p));
        server& serv = servers.back();
        while (true) {
            newToken = p.getToken();
            if (newToken == "}") {
                break;
            }
            if (newToken == "location") {
                hasLocation = true;
                string uri = p.getToken(); // validate URI!
                if (uri.empty() || uri == "{") {
                    throw std::runtime_error("Error: Missing uri at line " + std::to_string(p.getLineNum()));
                }
                locations.push_back(location(p, serv, uri));
                location& loc = locations.back();
                loc.set();
                serv.locations.insert(std::make_pair(locations.back().getUri(), &locations.back()));
                cout << "location uri is " << locations.back().getUri() << " with location add = " << &locations.back() << endl;
                continue ;
            }
            serv.setServerInfo(newToken);
        }
        if (serv.locations.find("/") == serv.locations.end()) {
            locations.push_back(location(p, serv, "/"));
            location& loc = locations.back();
            serv.locations.insert(std::make_pair("/", &loc));
        }
        serv.checkServerInfo();
        if (scopes.back() != SERVER) {
            throw std::runtime_error("Error: Missing closing bracket '}' at line " + std::to_string(p.getLineNum()));
        }
        scopes.pop_back();
    }
    // int count = 0;
    // string value = "";
    // string newToken = "";
    // while (true) {
    //     newToken = p.getToken();
    //     if (newToken == ";") {
    //         break;
    //     }
    //     if (count > 1) {
    //         value += " ";
    //     }
    //     value += newToken;
    //     count++;
    //     if (it->second && it->second < count) {
    //         throw locExp::TOO_MANY_ARGS();
    //     }
    // }
    // // if (newToken != ";") { // unnecessary!
    // //     throw locExp::DIRECT_NOT_VALID();
    // // }
    // info[token] = value;
}


void Config::print () {
    cout << "*************printing**************" << endl;
    cout << "server size is " << servers.size() << endl;
    for (size_t i = 0; i < servers.size(); i++) {
        cout << "server " << i << " server_name is ";
        for (size_t j = 0; j < servers[i].server_name.size(); j++) {
            cout << servers[i].server_name[j] << " ";
        }
        cout << endl;
        cout << "server " << i << " listenIp is " << servers[i].listenIp << endl;
        cout << "server " << i << " listenPort is " << servers[i].listenPort << endl;
        cout << "server " << i << " error_page is " << servers[i].error_page << endl;
        cout << "server " << i << " location size is " << servers[i].locations.size() << endl;
        cout << "here 1" << endl;
        for (map<string, location*>::iterator it = servers[i].locations.begin(); it != servers[i].locations.end(); it++) {
            cout << "here 2" << endl;
            cout << "URI is " << it->first << " or insde value " << it->second->getUri() << endl;
    //         cout << "server " << i << " location " << it->first << " root is " << it->second->info["root"] << endl;
    //         cout << "server " << i << " location " << it->first << " index is " << it->second->info["index"] << endl;
    //         cout << "server " << i << " location " << it->first << " autoindex is " << it->second->info["autoindex"] << endl;
    //         cout << "server " << i << " location " << it->first << " client_max_body_size is " << it->second->info["client_max_body_size"] << endl;
    //         cout << "server " << i << " location " << it->first << " cgi is " << it->second->info["cgi"] << endl;
    //         cout << "server " << i << " location " << it->first << " upload_store is " << it->second->info["upload_store"] << endl;
    //         cout << "server " << i << " location " << it->first << " upload_pass is " << it->second->info["upload_pass"] << endl;
    //         cout << "server " << i << " location " << it->first << " upload_pass_args is " << it->second->info["upload_pass_args"] << endl;
    //         cout << "server " << i << " location " << it->first << " methods is " << it->second->info["methods"] << endl;
    //         cout << "server " << i << " location " << it->first << " return is " << it->second->info["return"] << endl;
    //         cout << "server " << i << " location " << it->first << " auth_basic is " << it->second->info["auth_basic"] << endl;
    //         cout << "server " << i << " location " << it->first << " auth_basic_user_file is " << it->second->info["auth_basic_user_file"] << endl;
    //         cout << "server " << i << " location " << it->first << " access_log is " << it->second->info["access_log"] << endl;
    //     }
    }
    cout << "*************printing done**************" << endl;
}
}

int main() {
    // Parser p("../config/default.config");
    // while (true) {
    //     string token = p.getToken();
    //     if (token.empty())
    //         break;
    //     cout << "token is ->" << token << endl;
    // }
    Config conf("../config/default.config");
    conf.read();
    conf.print();

    return 0;
}

















Parser::Parser(const std::string& filePath) : lineNumber(0), linePos(0) {
    configFile.open(filePath.c_str(), std::ios::in);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << filePath << std::endl;
        throw std::runtime_error("Failed to open config file");
    }
}

Parser::~Parser() {
    configFile.close();
}

// bool ConfigParser::parse() {
//     try {
//         parseMainContext();
//     }
//     catch (std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         configFile.close();
//         return false;
//     }
// }

// void ConfigParser::parseMainContext() {
//     scopes.push_back(MAIN);
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == "server") {
//             parseServerContext();
//         }
//         else {
//             std::cerr << "Error: Invalid token '" << token << "' at line " << lineNumber << std::endl;
//             throw std::exception();
//         }
//     }
//     if (scopes.back() != MAIN) {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
// }

// void ConfigParser::parseServerContext() {
//     cout << "parseServerContext" << endl;
//     if (scopes.back() != MAIN) {
//         std::cerr << "Error: Invalid context 'server' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     if (findBracket("{"))
//         scopes.push_back(SERVER);
//     else {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     server.push_back(serverContext(*this));
//     cout << "adding new server " << server.size() << endl;
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == "server_name") {
//             server.back().setServerName();
//         }
//         else if (token == "listen") {
//             server.back().setLiten();
//         }
//         else if (token == "}") {
//             break;
//         }
//         else {
//             server.back().setLocation(token);
//         }
//     }
//     if (isEndingLine() == false) {
//         std::cerr << "Error: Invalid token '" << getToken() << "' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     if (scopes.back() != SERVER) {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     scopes.pop_back();
//     cout << "*************serverparsing done**************" << endl;
//     cout << "on this serever rout is " << server.back().location.find("/")->second.root << endl;
// }

bool Parser::findBracket(const std::string& bracket) {
    std::string token;
    while (true) {
        token = getToken();
        if (token.empty())
            break;
        if (token == bracket) {
            return true;
        }
        else {
            throw std::runtime_error("Error: Invalid token (need an opening bracket) '"
                                        + token + "' at line " + std::to_string(lineNumber));
        }
    }
    return false;
}

bool Parser::is_space(const char c) {
    if (c == ' ' || c == '\t')
        return true;
    return false;
}

bool Parser::is_delimitter(const char c) {
    if (c == '{' || c == '}' || c == ';')
        return true;
    return false;
}

bool Parser::isEndingLine() {
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

std::string Parser::getToken() {
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
            else if (line[i] == '}' || line[i] == ';')
                ;
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

int Parser::getLineNum() {
    return lineNumber;
}

std::vector<configScope>& Parser::getScopes() {
    return scopes;
}
