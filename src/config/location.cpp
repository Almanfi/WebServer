/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:48:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/18 04:40:56 by maboulkh         ###   ########.fr       */
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
    // directive.insert(std::make_pair("root", 1));
    // directive.insert(std::make_pair("index", 0));
    // directive.insert(std::make_pair("autoindex", 1));
    // directive.insert(std::make_pair("client_max_body_size", 1));
    // directive.insert(std::make_pair("cgi", 0));
    // directive.insert(std::make_pair("methods", 0));
    // directive.insert(std::make_pair("return", 1));
    // directive.insert(std::make_pair("error_page", 1));
    // directive.insert(std::make_pair("upload", 1));
    // directive.insert(std::make_pair("upload_path", 1));
    // directive.insert(std::make_pair("cgi_timeout", 1));
    // directive.insert(std::make_pair("cgi_path", 1));
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
    validationMap.insert(std::make_pair("cgi_path", &Location::validateCgiPath));
    validationMap.insert(std::make_pair("cgi_timeout", &Location::validateCgiTimeout));
    validationMap.insert(std::make_pair("upload", &Location::validateUpload));
    validationMap.insert(std::make_pair("upload_path", &Location::validateUploadPath));

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

void Location::validateUpload(const string& value) {
    if (value.empty() || (value != "on" && value != "off"))
        throw LocationException::INVALID_VALUE("upload", value);
}

void Location::validateUploadPath(const string& value) {
    if (value.empty())
        throw LocationException::INVALID_VALUE("upload_path", value);
    if (value[0] != '/')
        throw LocationException::INVALID_VALUE("upload_path", value);
    if (value.find(" ") != string::npos)
        throw LocationException::INVALID_VALUE("upload_path",
                        value + " (too many arguments)");
}

void Location::validateCgi(const string& value) {
    if (value.empty() || (value != "on" && value != "off"))
        throw LocationException::INVALID_VALUE("cgi", value);
}

void Location::validateCgiPath(const string& value) {
    if (value.empty())
        throw LocationException::INVALID_VALUE("cgi_path", value);
    if (value[0] != '/')
        throw LocationException::INVALID_VALUE("cgi_path", value);
    if (value.find(" ") != string::npos)
        throw LocationException::INVALID_VALUE("cgi_path",
                        value + " (too many arguments)");
}

void Location::validateCgiTimeout(const string& value) {
    stringstream ss(value);
    ssize_t val;
    ss >> val;
    if (value.empty() || ss.fail() || val < 0) {
        throw LocationException::INVALID_VALUE("cgi_timeout", value);
    }
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
    stringstream ss(value);
    int code;
    ss >> code;
    if (ss.fail() || code < 100 || code > 599)
        throw LocationException::INVALID_VALUE("return", value);
    string urlOrBody;
    ss >> urlOrBody;
    if (urlOrBody.empty() == false && ss.eof() == false)
        throw LocationException::INVALID_VALUE("return",
                        value + " (too many arguments)");
    
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
    info.insert(std::make_pair(key, value));
}


void Location::setNewLoc() {
    string uri = Parser::getTok(); // TODO validate URI!
    if (uri.empty() || uri == "{")
        throw LocationException::MISSING_LOCATION_URI();
    if (uri[0] != '/' && uri[0] != '*')
        throw LocationException::INVALID_LOCATION_URI(uri);
    map<string, Location>::iterator it;
    it = innerLocations.insert(std::make_pair(uri, Location(uri))).first;
    Location& loc = it->second;
    loc.setServer(*serv);
    loc.set();
    if (uri[0] == '*' && loc.getInfo("cgi_path").empty()) // TODO check for cgi necessary args
        throw LocationException::MISSING_CGI_PATH(uri);
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
    for (map<string, Location>::iterator it = innerLocations.begin();
                                    it != innerLocations.end(); it++) {
        if (it->first[0] != '*')
            continue;
        string extention = it->first.substr(1);
        if (location.find(extention) == std::string::npos)
            continue;
        return (it->second);
    }
    while (loc != "") {
        map<string, Location>::iterator it;
        it = innerLocations.find(loc);
        if (it != innerLocations.end()) {
            return (it->second.getLocation(location));
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

vector<string> Location::methods() {
    vector <string> methods;
    string meth = getInfo("methods");
    if (meth.empty()) {
        meth = Config::getDefault("methods");
    }
    stringstream ss(meth);
    string token;
    while (ss >> token) {
        methods.push_back(token);
    }
    return (methods);
}

string Location::root() {
    return (getInfo("root"));
}

vector<string> Location::index() {
    vector<string> index;
    string idx = getInfo("index");
    if (idx.empty())
        return (index);
    stringstream ss(idx);
    string token;
    while (ss >> token) {
        index.push_back(token);
    }
    return (index);
}

bool Location::autoindex() {
    string autoindex = getInfo("autoindex");
    if (autoindex.empty()) {
        autoindex = Config::getDefault("autoindex");
    }
    if (autoindex == "on")
        return (true);
    return (false);
}

int Location::returnCode() {
    string ret = getInfo("return");
    if (ret.empty()) {
        return (0);
    }
    stringstream ss(ret);
    int code;
    ss >> code;
    return (code);
}

string Location::returnUrl() {
    string ret = getInfo("return");
    if (ret.empty()) {
        return ("");
    }
    size_t pos = ret.find(" ");
    if (pos == string::npos) {
        return ("");
    }
    return (ret.substr(pos + 1));
}

bool Location::allowUpload() {
    // TODO what is the directive to set this? is it inside upload_dir?
    bool upload = true;
    return (upload);
}

string Location::uploadPath() {
    string path = getInfo("upload_path");
    if (path.empty()) {
        path = Config::getDefault("upload_path");
    }
    return (path);
}

bool Location::allowCGI() {
    string cgi = getInfo("cgi");
    if (cgi.empty()) {
        cgi = Config::getDefault("cgi");
    }
    if (cgi == "on")
        return (true);
    return (false);
}

string Location::CGIPath() {
    string path = getInfo("cgi_path");
    if (path.empty()) {
        path = Config::getDefault("cgi_path");
    }
    return (path);
}

size_t Location::CGITimeout() {
    string timeout = getInfo("cgi_timeout");
    if (timeout.empty()) {
        timeout = Config::getDefault("cgi_timeout");
    }
    stringstream ss(timeout);
    size_t time;
    ss >> time;
    return (time);
}