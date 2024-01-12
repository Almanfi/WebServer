/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:47:21 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/12 14:35:02 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

void Server::setAllowedDirective() {
    if (!directive.empty()) {
        return ;
    }
    directive.insert(std::make_pair("server_name", 0));
    directive.insert(std::make_pair("listen", 1));
    directive.insert(std::make_pair("error_page", 0));
}

map<string, int> Server::directive;

Server::Server(Config& c, Parser& p) : c(c), p(p) {
    setAllowedDirective();
}


Server::~Server() {
}

void Server::setMainLocation(string& token) {
    map<string, Location*>::iterator it = locations.find("/");
    if (it == locations.end()) {
        deque<Location>& allLoc = c.getLocations();
        allLoc.push_back(Location(c, *this, p, "/"));
        Location& loc = allLoc.back();
        locations.insert(std::make_pair("/", &loc));
    }
    Location& loc = *(locations.find("/")->second);
    loc.setLocationInfo(token);
}

void Server::setNewLocation() {
    deque<Location>& allLoc = c.getLocations();
    string uri = p.getToken(); // validate URI!
    if (uri.empty() || uri == "{") {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Missing uri at line " + ss.str());
    }
    allLoc.push_back(Location(c, *this, p, uri));
    Location& loc = allLoc.back();
    loc.set();
    locations.insert(std::make_pair(loc.getUri(), &loc));
}

void Server::setServerInfo(string& token) {
    if (token == "location") {
        setNewLocation();
        return ;
    }
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        setMainLocation(token);
        return ;
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
        if (count >= 1) {
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

const string& Server::validateIp(const string& ip) {
    std::stringstream ss(ip);
    int num;
    char dot;

    ss >> num;
    if (ss.fail() || num < 0 || num > 255)
        throw std::runtime_error("Invalid IP address");
    int i = 0;
    while (i++ < 3 && ss >> dot >> num) {
        if (ss.fail() || dot != '.')
            throw std::runtime_error("Invalid IP address");
        if (num < 0 || num > 255)
            throw std::runtime_error("Invalid IP address");
    }
    if (!ss.eof() || i != 4)
        throw std::runtime_error("Invalid IP address");
    return (ip);
}
int Server::validatePort(const string& portStr) {
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

void Server::parseListen() {
    string listen = info["listen"];
    size_t colon = listen.find(":");
    string ipStr;
    string portStr;
    if (colon == string::npos) {
        ipStr = "127.0.0.1";
        portStr = listen;
    }
    else {
        ipStr = listen.substr(0, colon);
        portStr = listen.substr(colon + 1);
    }
    listenIp = validateIp(ipStr);
    listenPort = validatePort(portStr);
    info.insert(std::make_pair("host", ipStr));
    info.insert(std::make_pair("port", portStr));
}

void Server::parseServerName() {
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

void Server::parseErrorPage() {
    if (info.find("error_page") == info.end()) {
        error_page = "";
        return ;
    }
    error_page = info["error_page"];
}

void Server::checkServerInfo() {
    if (info.find("listen") == info.end()) {
        throw locExp::LISTEN_NOT_FOUND();
    }
    parseListen();
    parseServerName();
    parseErrorPage();
}

void Server::linkLocation() {
    Location& rootLoc = *(locations.find("/")->second);
    for (map<string, Location*>::iterator it = locations.begin(); it != locations.end(); it++) {
        if (it->first == "/")
            continue ;
        rootLoc.addToInLoc(it->second);
    }
    rootLoc.propagate();
}

void Server::finalize() {
    deque<Location>& allLoc = c.getLocations();
    if (locations.find("/") == locations.end()) {
        allLoc.push_back(Location(c, *this, p, "/"));
        Location& loc = allLoc.back();
        locations.insert(std::make_pair("/", &loc));
    }
    linkLocation();
    checkServerInfo();
}

string Server::getInfo(const string& key) {
    if (info.find(key) != info.end())
        return (info[key]);
    return "";
}
