// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   configParser.cpp                                   :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/01/05 22:44:53 by maboulkh          #+#    #+#             */
// /*   Updated: 2024/01/05 22:50:50 by maboulkh         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "config.hpp"

// class ConfigParser {
// public:
//     ConfigParser(const std::string& filePath);

//     bool parse();

//     void parseMainContext();

//     void parseServerContext();

//     bool findBracket(const std::string& bracket);

//     bool is_space(const char c);

//     bool is_delimitter(const char c);

//     bool isEndingLine();

//     std::string getToken();

//     int getLineNum();

//     std::vector<configScope>& getScopes();

// private:
//     std::string filePath;
//     std::ifstream configFile;
//     int lineNumber;
//     std::string line;
//     size_t linePos;
//     std::vector<configScope> scopes;
//     std::map<std::string, std::string> configData;
//     std::map<std::string, std::string> mainContextData;
//     std::vector<serverContext> server;
// };

// ConfigParser::ConfigParser(const std::string& filePath) : filePath(filePath), lineNumber(0), linePos(0) { 
// }

// bool ConfigParser::parse() {
//     configFile.open(filePath.c_str(), std::ios::in);
//     if (!configFile.is_open()) {
//         std::cerr << "Failed to open config file: " << filePath << std::endl;
//         return false;
//     }
//     try {
//         parseMainContext();
//     }
//     catch (std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         configFile.close();
//         return false;
//     } 
//     configFile.close();
//     return true;
// }

// void ConfigParser::parseMainContext() {
//     scopes.push_back(MAIN);
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == "server") {
//             parseServerContext();
//         }
//         else {
//             std::cerr << "Error: Invalid token '" << token << "' at line " << lineNumber << std::endl;
//             throw std::exception();
//         }
//     }
//     if (scopes.back() != MAIN) {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
// }

// void ConfigParser::parseServerContext() {
//     if (scopes.back() != MAIN) {
//         std::cerr << "Error: Invalid context 'server' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     if (findBracket("{"))
//         scopes.push_back(SERVER);
//     else {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     server.push_back(serverContext(*this));
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == "server_name") {
//             server.back().setServerName();
//         }
//         else if (token == "listen") {
//             server.back().setLiten();
//         }
//         else if (token == "}") {
//             break;
//         }
//         else {
//             server.back().setLocation(token);
//         }
//     }
//     if (isEndingLine() == false) {
//         std::cerr << "Error: Invalid token '" << getToken() << "' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     if (scopes.back() != SERVER) {
//         std::cerr << "Error: Missing closing bracket '}' at line " << lineNumber << std::endl;
//         throw std::exception();
//     }
//     scopes.pop_back();
// }

// bool ConfigParser::findBracket(const std::string& bracket) {
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == bracket) {
//             return true;
//         }
//         else {
//             std::cerr << "Error: Invalid token (need an opening bracket) '" << token << "' at line " << lineNumber << std::endl;
//             throw std::exception();
//         }
//     }
//     return false;
// }

// bool ConfigParser::is_space(const char c) {
//     if (c == ' ' || c == '\t')
//         return true;
//     return false;
// }

// bool ConfigParser::is_delimitter(const char c) {
//     if (c == '{' || c == '}' || c == ';')
//         return true;
//     return false;
// }

// bool ConfigParser::isEndingLine() {
//     while (is_space(line[linePos]))
//         linePos++;
//     if (line[linePos] == '#' || linePos >= line.length()) {
//         std::getline(configFile, line);
//         lineNumber++;
//         linePos = 0;
//         return true;
//     }
//     return false;
// }

// std::string ConfigParser::getToken() {
//     size_t& i = linePos;
//     if (i >= line.length()) {
//         std::getline(configFile, line);
//         lineNumber++;
//         i = 0;
//     }
//     while (is_space(line[i]))
//         i++;
//     if (line[i] == '#' || i >= line.length()) {
//         i = line.length();
//         std::cout << "Comment : skip line!" << std::endl;
//         return getToken();
//     }
//     if (is_delimitter(line[i])) {
//         i++;
//         std::string delimitter = line.substr(i - 1, 1);
//         if (delimitter == ";") {
//             while (is_space(line[i]))
//                 i++;
//             if (line[i] == '#' || i >= line.length()) {
//                 i = line.length();
//                 std::cout << "Comment : skip line!" << std::endl;
//             }
//             else {
//                 std::cerr << "Error: found token after ';' " << " at line " << lineNumber << std::endl;
//                 throw std::exception();
//             }
//         }
//         return delimitter;
//     }
//     size_t start = i;
//     while (!is_space(line[i]) && !is_delimitter(line[i]) && line[i] != '\0')
//         i++;
//     return line.substr(start, i - start);
// }

// int ConfigParser::getLineNum() {
//     return lineNumber;
// }

// std::vector<configScope>& ConfigParser::getScopes() {
//     return scopes;
// }
