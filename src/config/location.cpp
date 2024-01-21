/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:48:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/21 18:02:23 by maboulkh         ###   ########.fr       */
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
    directive.insert(std::make_pair("methods", 0));
    directive.insert(std::make_pair("return", 1));
    directive.insert(std::make_pair("error_page", 1));
}

map<string, void (Location::*) (const string&)> Location::validationMap;
vector<string> Location::httpAllowedMethods;

void Location::initValidationMap() {
    if (!validationMap.empty()) {
        return ;
    }
    validationMap.insert(std::make_pair("root", &Location::validateRoot));
    validationMap.insert(std::make_pair("index", &Location::validateIndex));
    validationMap.insert(std::make_pair("autoindex", &Location::validateAutoindex));
    validationMap.insert(std::make_pair("client_max_body_size", &Location::validateClientMaxBodySize));
    validationMap.insert(std::make_pair("cgi", &Location::validateCgi));
    validationMap.insert(std::make_pair("methods", &Location::validateMethods));
    validationMap.insert(std::make_pair("return", &Location::validateReturn));
    validationMap.insert(std::make_pair("error_page", &Location::validateErrorPage));

    httpAllowedMethods.push_back("GET");
    httpAllowedMethods.push_back("POST");
    httpAllowedMethods.push_back("DELETE");
}

void Location::validateDirective(const string& key, const string& value) {
    map<string, void (Location::*) (const string&)>::iterator it = validationMap.find(key);
    if (it == validationMap.end()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    (this->*(it->second))(value);
}

void Location::validateRoot(const string& value) {
    if (value.empty() || value[0] != '/') {
        throw std::runtime_error("Error: Invalid path");
    }
    if (value.find(" ") != string::npos) {
        throw std::runtime_error("Error: path has too many args");
    }
}

void Location::validateIndex(const string& value) {
    if (value.empty()) {
        throw locExp::DIRECT_NOT_VALID();
    }
}

void Location::validateAutoindex(const string& value) {
    if (value != "on" && value != "off") {
        throw locExp::DIRECT_NOT_VALID();
    }
}

void Location::validateClientMaxBodySize(const string& value) {
    stringstream ss(value);
    ssize_t val;
    ss >> val;
    if (value.empty() || ss.fail() || val < 0) {
        throw locExp::DIRECT_NOT_VALID();
    }
}

void Location::validateCgi(const string& value) {
    if (value.empty() || (value != "on" && value != "off")) {
        throw locExp::DIRECT_NOT_VALID();
    }
}
//TODO come back add allowed methods
void Location::validateMethods(const string& value) {
    if (value.empty()) {
        throw locExp::DIRECT_NOT_VALID();
    }
    stringstream ss(value);
    vector<string>& allowed = httpAllowedMethods;
    string method;
    while (ss >> method) {
        if (method.empty()
            || std::find(allowed.begin(), allowed.end(), method) == allowed.end()) {
            throw std::runtime_error("Error: Invalid method");
        }
    }
}

void Location::validateReturn(const string& value) {
    if (value.empty()) {
        throw locExp::DIRECT_NOT_VALID();
    }
}

void Location::validateErrorPage(const string& value) {
    stringstream ss(value);
    int val;
    int count = 0;
    while (ss >> val) {
        count++;
        if (val < 300 || val > 599)
            throw std::runtime_error("Error: Invalid status code");
    }
    if (count == 0)
        throw std::runtime_error("no status code");
    ss.clear();
    string path;
    ss >> path;
    if (ss.fail() || path.empty() || path[0] != '/') {
        throw std::runtime_error("Error: Invalid path");
    }
    string extra;
    ss >> extra;
    if (!extra.empty() || !ss.eof()) {
        throw std::runtime_error("Error: extra arguments for error_page");
    }
}

void Location::insertDirective(const string& key, const string& value) {
    validateDirective(key, value);
    KeyVal::iterator it = info.find(key);
    if (it != info.end()) {
        if (key == "error_page") {
            it->second += " ; " + value;
            return ;
        }
        throw locExp::DIRECT_ALREADY_SET();
    }
    info.insert(std::make_pair(key, value));
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
    if (uri[0] != '/') {
        stringstream ss;
        ss << p.getLineNum();
        throw std::runtime_error("Error: Invalid uri at line " + ss.str());
    }
    allLoc.push_back(Location(c, serv, p, uri));
    Location& loc = allLoc.back();
    loc.set();
    addToInLoc(&loc);
}

void Location:: set() {
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
}

string& Location::getUri() const {
    return (const_cast<string&>(uri));
}

void Location::setLocationInfo(string& token) {
    if (token == "location") {
        setNewLoc();
        return ;
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
    insertDirective(token, value);
    token = newToken;
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
            KeyVal::iterator infoIt = inLoc[i]->info.find(it->first);
            if (infoIt == inLoc[i]->info.end())
                inLoc[i]->info.insert(std::make_pair(it->first, it->second));
            else if (it->first == "error_page") {
                infoIt->second += " ; " + it->second;
            }
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
    cout << sp << "   *methods\t\t\tis " << getInfo("methods") << endl;
    cout << sp << "   *return\t\t\tis " << getInfo("return") << endl;
    cout << sp << "   *error page\t\t\tis " << getInfo("error_page") << endl;
}

string Location::getErrorPage(const string& code) {
    string errorPageStr = getInfo("error_page");
    size_t pos = errorPageStr.find(code);
    if (pos == string::npos) {
        return ("");
    }
    size_t start = errorPageStr.find("/", pos);
    size_t end = errorPageStr.find(" ", start);
    return (errorPageStr.substr(start, end - start));
}

bool Location::isAllowedMethod(const string& method) {
    string methods = getInfo("methods");
    if (methods.empty())
        methods = c.getDefault().find("methods")->second;
    if (methods.find(method) == string::npos)
        return (false);
    return (true);
}