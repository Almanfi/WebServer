/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:47:21 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 23:24:47 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

void Server::setAllowedDirective() {
    if (!directive.empty()) {
        return ;
    }
    directive.insert(std::make_pair("server_name", 0));
    directive.insert(std::make_pair("listen", 1));
}

map<string, int> Server::directive;

Server::Server() : rootLocation("/") {
    setAllowedDirective();
}

Server::~Server() {
}

void Server::set() {
    string newToken = Parser::getTok();
    if (newToken != "{")
        throw ConfigException::MISSING_BRACKET("{");
    rootLocation.setServer(*this);
    while (true) {
        newToken = Parser::getTok();
        if (newToken == "}" || newToken.empty())
            break;
        if (newToken == ";")
            continue;
        setServerInfo(newToken);
    }
    if (newToken != "}")
        throw ConfigException::MISSING_BRACKET("}");
    finalize();
}

void Server::setMainLocation(string& token) {
    rootLocation.setLocationInfo(token);
}

void Server::setServerInfo(string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        rootLocation.setLocationInfo(token);
        return ;
    }
    if (info.find(token) != info.end())
        throw ConfigException::DIRECT_ALREADY_SET(token);
    int count = 0;
    string value = "";
    string newToken = "";
    while (true) {
        newToken = Parser::getTok();
        if (newToken == ";" || newToken.empty()) {
            break;
        }
        if (count >= 1) {
            value += " ";
        }
        value += newToken;
        count++;
        if (it->second && it->second < count)
            throw ConfigException::TOO_MANY_ARGS(token);
    }
    info[token] = value;
    token = newToken;
}

const string& Server::validateIp(const string& ip) {
    // std::stringstream ss(ip);
    // int num;
    // char dot;

    // ss >> num;
    // if (ss.fail() || num < 0 || num > 255)
    //     throw std::runtime_error("Invalid IP address");
    // int i = 0;
    // while (i++ < 3 && ss >> dot >> num) {
    //     if (ss.fail() || dot != '.')
    //         throw std::runtime_error("Invalid IP address");
    //     if (num < 0 || num > 255)
    //         throw std::runtime_error("Invalid IP address");
    // }
    // if (!ss.eof() || i != 4)
    //     throw std::runtime_error("Invalid IP address");
    return (ip);
}
int Server::validatePort(const string& portStr) {
    if (portStr[0] == '+' || portStr[0] == '-'
        || (portStr[0] == '0' && portStr.size() > 1))
        throw ServerException::NOT_VALID_PORT(portStr);
    std::stringstream ss(portStr);
    int port;
    ss >> port;
    if (ss.fail() || !ss.eof() || port < 0 || port > 65535)
        throw ServerException::NOT_VALID_PORT(portStr);
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
        throw ServerException::LISTEN_NOT_FOUND();
    }
    parseListen();
    parseServerName();
    parseErrorPage();
}

void Server::finalize() {
    checkServerInfo();
    rootLocation.propagate();
}

string Server::getInfo(const string& key) const {
    map<string, string>::const_iterator it = info.find(key);
    if (it != info.end())
        return (it->second);
    return "";
}

Location& Server::getRootLocation() {
    return rootLocation;
}

Location& Server::getLocation(const string& uri) {
    return rootLocation.getLocation(uri);
}