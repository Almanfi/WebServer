/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:48:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/14 18:55:25 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config.hpp"

Location::Location(const string& uri) : uri(uri), serv(NULL) {
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
        throw ConfigException::DIRECT_NOT_VALID(key);
    }
    (this->*(it->second))(value);
}

void Location::validateRoot(const string& value) {
    if (value.empty() || value[0] != '/') {
        throw LocationException::INVALID_ROOT(value);
    }
    if (value.find(" ") != string::npos) {
        throw LocationException::INVALID_ROOT(value
                            + " (too many arguments)");
    }
}

void Location::validateIndex(const string& value) {
    if (value.empty()) {
        throw LocationException::INVALID_VALUE("index", value);
    }
}

void Location::validateAutoindex(const string& value) {
    if (value != "on" && value != "off") {
        throw LocationException::INVALID_VALUE(
                            "autoindex", value);
    }
}

void Location::validateClientMaxBodySize(const string& value) {
    stringstream ss(value);
    ssize_t val;
    ss >> val;
    if (value.empty() || ss.fail() || val < 0) {
        throw LocationException::INVALID_VALUE(
                            "client_max_body_size", value);
    }
}

void Location::validateCgi(const string& value) {
    if (value.empty() || (value != "on" && value != "off"))
        throw LocationException::INVALID_VALUE("cgi", value);
}
//TODO come back add allowed methods
void Location::validateMethods(const string& value) {
    if (value.empty())
        throw LocationException::INVALID_METHOD("(empty)");
    stringstream ss(value);
    vector<string>& allowed = httpAllowedMethods;
    string method;
    while (ss >> method) {
        if (method.empty()
            || std::find(allowed.begin(), allowed.end(), method) == allowed.end()) {
            throw LocationException::INVALID_METHOD(method);
        }
    }
}

void Location::validateReturn(const string& value) {
    if (value.empty()) {
        throw LocationException::INVALID_VALUE("return", value);
    }
}

void Location::validateErrorPage(const string& value) {
    stringstream ss(value);
    int val;
    int count = 0;
    while (ss >> val) {
        count++;
        if (val < 300 || val > 599)
            throw LocationException::INVALID_VALUE("error_page", value);
    }
    if (count == 0)
        throw LocationException::INVALID_VALUE("error_page", ("(no code)"));
    ss.clear();
    string path;
    ss >> path;
    if (ss.fail() || path.empty() || path[0] != '/') {
        throw LocationException::INVALID_VALUE("error_page", path);
    }
    string extra;
    ss >> extra;
    if (!extra.empty() || !ss.eof()) {
        throw LocationException::INVALID_VALUE(
                            "error_page", value + " (too many arguments)");
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
        throw LocationException::DIRECT_ALREADY_SET(key);
    }
    if (key == "root" && value.back() == '/') { // TODO check this, this is for concatination of root and uri
        info.insert(std::make_pair(key, value.substr(0, value.size() - 1)));
        return ;
    }
    info.insert(std::make_pair(key, value));
}


void Location::setNewLoc() {
    string uri = Parser::getTok(); // validate URI!
    if (uri.empty() || uri == "{")
        throw LocationException::MISSING_LOCATION_URI();
    if (uri[0] != '/')
        throw LocationException::INVALID_LOCATION_URI(uri);
    map<string, Location>::iterator it;
    it = innerLocations.insert(std::make_pair(uri, Location(uri))).first;

    Location& loc = it->second;
    loc.setServer(*serv);
    loc.set();
}
void Location::setServer(Server& servRef) {
    serv = &servRef;
}

void Location:: set() {
    if (Parser::getTok() != "{")
        throw ConfigException::MISSING_BRACKET("{");
    std::string token;
    while (true) {
        token = Parser::getTok();
        if (token == "}" || token.empty()) {
            break;
        }
        if (token == ";")
            continue;
        setLocationInfo(token);
    }
    if (token != "}" || token.empty())
        throw ConfigException::MISSING_BRACKET("}");
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
        newToken = Parser::getTok();
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
    return (serv->getInfo(key));
}

void Location::print(int space) {
    printThis(space);
    for (map<string, Location>::iterator it = innerLocations.begin(); it != innerLocations.end(); it++) {
        cout << "____________________ prionting innerLocations " << it->first << " ____________________" << endl;
        it->second.print(space + 1);
    }
}

void Location::propagate() {
    for (map<string, Location>::iterator it = innerLocations.begin();
                                    it != innerLocations.end(); it++) {
        Location& loc = it->second;
        for (KeyVal::const_iterator itr = info.begin(); itr != info.end(); itr++) {
            KeyVal::iterator infoIt = loc.info.find(itr->first);
            if (infoIt == loc.info.end())
                loc.info.insert(std::make_pair(itr->first, itr->second));
            else if (itr->first == "error_page") {
                infoIt->second += " ; " + itr->second;
            }
        }
        loc.propagate();
    }
}

Location& Location::getLocation(const string& location) {
    string loc = location;
    while (loc != "/") {
        map<string, Location>::iterator it;
        it = innerLocations.find(loc);
        if (it != innerLocations.end()) {
            return (it->second.getLocation(location));
        }
        size_t pos = loc.rfind("/");
        if (pos == string::npos)
            pos = 0;
        loc = loc.substr(0, pos);
        if (location.back() == '/') // TODO check if this is correct
            loc += "/";
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

string Location::getErrorPage(const int code) {
    stringstream ss;
    ss << code;
    return (getErrorPage(ss.str()));
}

bool Location::isAllowedMethod(const string& method) {
    string methods = getInfo("methods");
    if (methods.empty()) // TODO add a default value for methods
        methods = Config::getDefault("methods");
    if (methods.find(method) == string::npos)
        return (false);
    return (true);
}