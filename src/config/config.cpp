/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 21:29:02 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/12 20:48:27 by maboulkh         ###   ########.fr       */
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
    instance.isInitialized = true;
    instance.read();
    return instance;
}

void Config::destroy() {
    Parser::destroy();
}

Config::Config() : isInitialized(false) {
}

void Config::initRules() {
    Location::initValidationMap();
    setAlloedDirective();
    defaultConfig.insert(std::make_pair("root", "www"));
    defaultConfig.insert(std::make_pair("index", "index.html"));
    defaultConfig.insert(std::make_pair("autoindex", "on"));
    defaultConfig.insert(std::make_pair("client_max_body_size", "1m"));
    defaultConfig.insert(std::make_pair("cgi", "off"));
    defaultConfig.insert(std::make_pair("methods", "GET"));
}

Config::~Config() {
    Parser::destroy();
}

map<string, int> Config::directive;

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
    if (!serv) {
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
        // cout << "server " << i << " location size is " << servers[i].locations.size() << endl;
        servers[i].getRootLocation().print(0);
    cout << "*************printing done**************" << endl;
    }
}
