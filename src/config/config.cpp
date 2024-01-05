/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:52:48 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/05 22:19:28 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;


typedef enum e_configScope {
    MAIN,
    SERVER,
    LOCATION
} configScope;



class ConfigParser;
class serverContext;
class locationContext;

class ConfigParser {
public:
    ConfigParser(const std::string& filePath) : filePath(filePath), lineNumber(0), linePos(0) { 
    }

    bool parse() {
        configFile.open(filePath.c_str(), std::ios::in);
        if (!configFile.is_open()) {
            std::cerr << "Failed to open config file: " << filePath << std::endl;
            return false;
        }
        try {
            parseMainContext();
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            configFile.close();
            return false;
        } 
        configFile.close();
        return true;
    }

    void parseMainContext() {
        scopes.push_back(MAIN);
        string token;
        while (true) {
            token = getToken();
            if (token.empty())
                break;
            if (token == "server") {
                parseServerContext();
            }
            else {
                std::cerr << "Error: Invalid token '" << token << "' at line " << lineNumber << std::endl;
                throw std::exception();
            }
        }
        if (scopes.back() != MAIN) {
            std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
            throw std::exception();
        }
    }

    void parseServerContext() {
        if (scopes.back() != MAIN) {
            std::cerr << "Error: Invalid context 'server' at line " << lineNumber << std::endl;
            throw std::exception();
        }
        if (findBracket("{"))
            scopes.push_back(SERVER);
        else {
            std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
            throw std::exception();
        }
        server.push_back(serverContext(*this));
        string token;
        while (true) {
            token = getToken();
            if (token.empty())
                break;
            if (token == "server_name") {
                server.back().setServerName();
            }
            else if (token == "listen") {
                server.back().setLiten();
            }
            else if (token == "}") {
                break;
            }
            else {
                server.back().setLocation(token);
            }
        }
        if (isEndingLine() == false) {
            std::cerr << "Error: Invalid token '" << getToken() << "' at line " << lineNumber << std::endl;
            throw std::exception();
        }
        if (scopes.back() != SERVER) {
            std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
            throw std::exception();
        }std::vector<configScope> scopes;
        scopes.pop_back();
    }

    bool findBracket(const string& bracket) {
        string token;
        while (true) {
            token = getToken();
            if (token.empty())
                break;
            if (token == bracket) {
                return true;
            }
            else {
                std::cerr << "Error: Invalid token (need an opening bracket) '" << token << "' at line " << lineNumber << std::endl;
                throw std::exception();
            }
        }
        return (false);
    }


    bool is_space(const char c) {
        if (c == ' ' || c == '\t')
            return true;
        return false;
    }
    
    bool is_delimitter(const char c) {
        if (c == '{' || c == '}' || c == ';')
            return true;
        return false;
    }

    bool isEndingLine(void) {
        while (is_space(line[linePos]))
            linePos++;
        if (line[linePos] == '#' || linePos >= line.length()) {
            std::getline(configFile, line);
            lineNumber++;
            linePos = 0;
            return (true);
        }
        return (false);
    }
    
    string getToken(void) {
        size_t& i = linePos;
        if (i >= line.length()) {
            // if (ignoreError)
            //     ignoreError = false;
            // if (lineEnded == false) {
            //     std::cerr << "Error: Missing ';' at line " << lineNumber << std::endl;
            //     throw std::exception();
            // }
            std::getline(configFile, line);
            lineNumber++;
            // lineEnded = false;
            i = 0;
        }
        while (is_space(line[i]))
            i++;
        if (line[i] == '#' || i >= line.length()) {
            i = line.length();
            cout << "Comment : skip line!" << endl;
            return (getToken());
        }
        if (is_delimitter(line[i])) {
            i++;
            string delimitter = line.substr(i - 1, 1);
            if (delimitter == ";") {
                while (is_space(line[i]))
                    i++;
                if (line[i] == '#' || i >= line.length()) {
                    i = line.length();
                    cout << "Comment : skip line!" << endl;
                }
                else {
                    std::cerr << "Error: found token after ';' " << " at line " << lineNumber << std::endl;
                    throw std::exception();
                }
            }
            return (delimitter);
        }
        size_t start = i;
        while (!is_space(line[i]) && !is_delimitter(line[i]) && line[i] != '\0')
            i++;
        return (line.substr(start, i - start));
    }
    int getLineNum() {
        return (lineNumber);
    }
    // void ignore(void) {
    //     ignoreError = true;
    // }
    std::vector<configScope>& getScopes() {
        return (scopes);
    }

private:
    std::string filePath;
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    // bool lineEnded;
    // bool ignoreError;
    std::vector<configScope> scopes;
    std::map<std::string, std::string> configData;
    std::map<std::string, std::string> mainContextData;
    std::vector<serverContext> server;
    
    // static const std::vector<string> mainContextWords;
    // static const std::vector<string> serverContextWords;
    // static const std::vector<string> locationContextWords;
};

class serverContext {
public:
    serverContext(ConfigParser &parent) : parent(parent) {
        listenPort = 80;
        listenIp = "";
        error_page = "";
        location.insert(std::pair<string, locationContext>("/", locationContext(parent, *this)));
    }
    ~serverContext() {}
    
    void setServerName() {
        string token;
        while (true) {
            token = parent.getToken();
            if (token == ";") {
                break;
            }
            else {
                server_name.push_back(token);
            }
        }
        if (server_name.empty()) {
            server_name.push_back("");
        }
    }
    void setLiten() {
        string token;
        token = parent.getToken();
        if (token.find(":") != string::npos) {
            listenIp = token.substr(0, token.find(":"));
            listenPort = std::stoi(token.substr(token.find(":") + 1));
        }
        else {
            listenIp = "";
            listenPort = std::stoi(token);
        }
        token = parent.getToken();
        if (token != ";") {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        // validate port number and ip address
        // if (listenPort < 0 || listenPort > 65535) {
        //     std::cerr << "Error: Invalid port number '" << token << "' at line " << parent.getLineNum() << std::endl;
        //     throw std::exception();
        // }
        
    }
    void setErrorPage() {
        string token;
        token = parent.getToken();
        if (token != ";") {
            error_page = token;
            token = parent.getToken();
            if (token != ";") {
                std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
                throw std::exception();
            }
        }
        else {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
    }
    void createNewLocation() {
        locationContext newLocation(parent, *this);
        newLocation.parseLocationContext();
        location.insert(std::pair<string, locationContext>(newLocation.root, newLocation));
    }
    
    bool setLocation(const string& token) {
        if (token == "location") {
            createNewLocation();
            return (true);
        }
        else {
            locationContext& loc = location.find("/")->second;
            loc.setLocationData(token);
        }
    }
    vector<string> server_name;
    string  listenIp;
    int     listenPort;
    std::string error_page;
    map<string, locationContext> location;
private:
    ConfigParser &parent;
};

class locationContext {
public:
    locationContext(ConfigParser &parent, serverContext &server) : parent(parent), server(server), scopes(parent.getScopes()) {
        uri = "/";
        root = "";
        index = "";
        autoindex = false;
        client_max_body_size = 0;
        cgi = "";
        upload_store = "";
        upload_pass = "";
        upload_pass_args = "";
        methods = "";
        return_val = "";
        auth_basic = "";
        auth_basic_user_file = "";
    }
    ~locationContext() {}
    
    void setUri() {
        string token;
        token = parent.getToken();
        if (token.empty() && token[0] != '/') {
            std::cerr << "Error: not a valid URI '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        uri = token;
    }

    void parseLocationContext() {
        if (scopes.back() != LOCATION || scopes.back() != SERVER) {
            std::cerr << "Error: Invalid context 'location' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        setUri();
        if (parent.findBracket("{"))
            scopes.push_back(LOCATION);
        else {
            std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        string token;
        while (true) {
            token = parent.getToken();
            if (token.empty())
                break;
            else if (token == "}")
                break;
            setLocationData(token);
            }
        if (parent.isEndingLine() == false) {
            std::cerr << "Error: Invalid token '" << parent.getToken() << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        if (scopes.back() != LOCATION) {
            std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        scopes.pop_back();
    }

    void setLocationData(const string& token) {
        if (token == "root") {
            root = parent.getToken();
        }
        else if (token == "index") {
            index = parent.getToken();
        }
        else if (token == "autoindex") {
            string token = parent.getToken();
            if (token == "on") {
                autoindex = true;
            }
            else if (token == "off") {
                autoindex = false;
            }
            else {
                std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
                throw std::exception();
            }
        }
        else if (token == "client_max_body_size") {
            string token = parent.getToken();
            client_max_body_size = std::stoi(token);
        }
        else if (token == "cgi") {
            cgi = parent.getToken();
        }
        else if (token == "upload_store") {
            upload_store = parent.getToken();
        }
        else if (token == "upload_pass") {
            upload_pass = parent.getToken();
        }
        else if (token == "upload_pass_args") {
            upload_pass_args = parent.getToken();
        }
        else if (token == "methods") {
            methods = parent.getToken();
        }
        else if (token == "return") {
            return_val = parent.getToken();
        }
        else if (token == "auth_basic") {
            auth_basic = parent.getToken();
        }
        else if (token == "auth_basic_user_file") {
            auth_basic_user_file = parent.getToken();
        }
        else {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
        if (parent.getToken() != ";") {
            std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
            throw std::exception();
        }
    }
    std::string uri;
    std::string root;
    std::string index;
    bool autoindex;
    int client_max_body_size;
    std::string cgi;
    std::string upload_store;
    std::string upload_pass;
    std::string upload_pass_args;
    std::string methods;
    std::string return_val;
    std::string auth_basic;
    std::string auth_basic_user_file;
private:
    ConfigParser &parent;
    serverContext &server;
    std::vector<configScope>& scopes;
};

// const std::vector<string> ConfigParser::mainContextWords = {"server", "location"};
// const std::vector<string> ConfigParser::serverContextWords = {"listen", "server_name", "error_page", "location", "root", "index", "autoindex", "client_max_body_size", "cgi", "upload_store", "upload_pass", "upload_pass_args", "methods", "return", "auth_basic", "auth_basic_user_file"};
// const std::vector<string> ConfigParser::locationContextWords = {"root", "index", "autoindex", "client_max_body_size", "cgi", "upload_store", "upload_pass", "upload_pass_args", "methods", "return", "auth_basic", "auth_basic_user_file"};

int main() {
    ConfigParser parser("./config/default.config");
    if (parser.parse()) {
        std::cout << "parsing done"<< std::endl;
    }

    return 0;
}
