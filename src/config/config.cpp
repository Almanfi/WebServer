/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 21:29:02 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/10 16:02:35 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

Config::Config(const std::string& filePath) : p(filePath) {
    setAlloedDirective();
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
    if (scopes.back() != MAIN) {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Missing closing bracket '}' at line " + ss.str());
    }
    scopes.pop_back();
}


void Config::setServer() {
        vector<configScope>& scopes = p.getScopes();
        scopes.push_back(SERVER);
        string newToken = p.getToken();
        if (newToken != "{") {
            stringstream ss;
            ss << p.getLineNum();
            throw std::runtime_error("Error: Missing opening bracket '{' at line " + ss.str());
        }
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
        if (scopes.back() != SERVER || newToken != "}") {
            stringstream ss;
            ss << p.getLineNum();
            throw std::runtime_error("Error: Missing closing bracket '}' at line " + ss.str());
        }
        scopes.pop_back();
}   

void Config::set(const string& token) {
    map<string, int>::iterator it = directive.find(token);
    if (it == directive.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    // if (info.find(token) != info.end()) {
    //     throw locExp::DIRECT_ALREADY_SET();
    // }
    if (token == "server") {
        setServer();
        return ;
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

deque<Location>& Config::getLocations() {
    return (locations);
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
        servers[i].locations.find("/")->second->print(0);
    cout << "*************printing done**************" << endl;
    }
}
