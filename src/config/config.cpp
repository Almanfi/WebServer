/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elasce <elasce@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 21:29:02 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/28 13:46:07 by elasce           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

Config::Config(const std::string& filePath) : p(filePath) {
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
}

void Config::readMainContext() {
    vector<configScope>& scopes = p.getScopes();
    scopes.push_back(MAIN);
    std::string token;
    while (true) {
        token = p.getToken();
        if (token.empty())
            break;
        if (token == ";")
            continue;
        set(token);
    }
    if (scopes.back() != MAIN)
        throw ConfigException::MISSING_BRACKET("}");
    scopes.pop_back();
}

void Config::setServer() {
        vector<configScope>& scopes = p.getScopes();
        scopes.push_back(SERVER);
        string newToken = p.getToken();
        if (newToken != "{")
            throw ConfigException::MISSING_BRACKET("{");
        servers.push_back(Server(*this, p));
        Server& serv = servers.back();
        while (true) {
            newToken = p.getToken();
            if (newToken == "}" || newToken.empty())
                break;
            if (newToken == ";")
                continue;
            serv.setServerInfo(newToken);
        }
        serv.finalize();
        if (scopes.back() != SERVER || newToken != "}")
            throw ConfigException::MISSING_BRACKET("}");
        scopes.pop_back();
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

deque<Location>& Config::getLocations() {
    return (locations);
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
    return (serv->locations.find("/")->second->getLocation(location));
}

deque<Server>& Config::getServers() {
    return (servers);
}

const KeyVal& Config::getDefault() const {
    return (defaultConfig);
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
        servers[i].locations.find("/")->second->print(0);
    cout << "*************printing done**************" << endl;
    }
}
