/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:48:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/19 21:12:32 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

Location::Location(Config& c, Server& serv, Parser& p, const string& uri)
    : c(c), serv(serv), p(p), uri(uri) {
    setAlloedDirective();
}

Location::~Location() {
}

map<string, int> Location::directive;

void Location::setAlloedDirective() {
    if (!directive.empty()) {
        return ;
    }
    directive.insert(std::make_pair("root", 1));
    directive.insert(std::make_pair("index", 0));
    directive.insert(std::make_pair("autoindex", 1));
    directive.insert(std::make_pair("client_max_body_size", 1));
    directive.insert(std::make_pair("cgi", 0));
    directive.insert(std::make_pair("upload_store", 1));
    directive.insert(std::make_pair("upload_pass", 1));
    directive.insert(std::make_pair("upload_pass_args", 1));
    directive.insert(std::make_pair("methods", 0));
    directive.insert(std::make_pair("return", 1));
    directive.insert(std::make_pair("auth_basic", 1));
    directive.insert(std::make_pair("auth_basic_user_file", 1));
    directive.insert(std::make_pair("access_log", 1));
}

void Location::addToInLoc(Location* loc) {
    inLoc.push_back(loc);
}

void Location::setNewLoc() {
    deque<Location>& allLoc = c.getLocations();
    string uri = p.getToken(); // validate URI!
    if (uri.empty() || uri == "{") {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Missing uri at line " + ss.str());
    }
    allLoc.push_back(Location(c, serv, p, uri));
    Location& loc = allLoc.back();
    loc.set();
    addToInLoc(&loc);
}

void Location::set() {
    vector<configScope>& scope = p.getScopes();
    scope.push_back(LOCATION);
    if (p.getToken() != "{") {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Missing opening bracket '{' at line " + ss.str());
    }
    std::string token;
    while (true) {
        token = p.getToken();
        if (token == "}" || token.empty()) {
            break;
        }
        if (token == ";")
            continue;
        setLocationInfo(token);
    }
    if (scope.back() != LOCATION || token.empty()) {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Missing closing bracket '}' at line " + ss.str());
    }
    scope.pop_back();
    checkLocationInfo();
}

string& Location::getUri() const {
    return (const_cast<string&>(uri));
}

void Location::setLocationInfo(string& token) {
    if (token == "location") {
        setNewLoc();
        return ;
    }
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
        if (count >= 1) {
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

void Location::checkLocationInfo() {
}


string Location::getInfo(const string& key) {
    map<string, string>::iterator it = info.find(key);
    if (it != info.end())
        return (it->second);
    return (serv.getInfo(key));
}

void Location::print(int space) {
    printThis(space);
    for (size_t i = 0; i < inLoc.size(); i++) {
        cout << "____________________ prionting inLoc " << i << " ____________________" << endl;
        inLoc[i]->print(space + 1);
    }
}

void Location::propagate() {
    for (size_t i = 0; i < inLoc.size(); i++) {
        for (map<string, string>::const_iterator it = info.begin(); it != info.end(); it++) {
            if (inLoc[i]->info.find(it->first) == inLoc[i]->info.end())
                inLoc[i]->info.insert(std::make_pair(it->first, it->second));
        }
        inLoc[i]->propagate();
    }
}

Location& Location::getLocation(const string& location) {
    string loc = location;
    while (loc != "") {
        for (size_t i = 0; i < inLoc.size(); i++) {
            if (inLoc[i]->uri == loc) {
                return (inLoc[i]->getLocation(location));
            }
        }
        size_t pos = loc.rfind("/");
        if (pos == string::npos)
            pos = 0;
        loc = loc.substr(0, pos);
    }
    return (*this);
}

void Location::printThis(int space) {
        string sp = "";
    for (int i = 0; i < space; i++)
        sp += "\t";
    cout << sp << "URI is " << uri << endl;
    cout << sp << "   *root\t\t\tis " << getInfo("root") << endl;
    cout << sp << "   *index\t\t\tis " << getInfo("index") << endl;
    cout << sp << "   *autoindex\t\t\tis " << getInfo("autoindex") << endl;
    cout << sp << "   *client_max_body_size\tis " << getInfo("client_max_body_size") << endl;
    cout << sp << "   *cgi\t\t\t\tis " << getInfo("cgi") << endl;
    cout << sp << "   *upload_store\t\tis " << getInfo("upload_store") << endl;
    cout << sp << "   *upload_pass\t\t\tis " << getInfo("upload_pass") << endl;
    cout << sp << "   *upload_pass_args\t\tis " << getInfo("upload_pass_args") << endl;
    cout << sp << "   *methods\t\t\tis " << getInfo("methods") << endl;
    cout << sp << "   *return\t\t\tis " << getInfo("return") << endl;
    cout << sp << "   *auth_basic\t\t\tis " << getInfo("auth_basic") << endl;
    cout << sp << "   *auth_basic_user_file\tis " << getInfo("auth_basic_user_file") << endl;
    cout << sp << "   *access_log\t\t\tis " << getInfo("access_log") << endl;
}