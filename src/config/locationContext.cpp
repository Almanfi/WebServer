// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   locationContext.cpp                                :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/01/05 23:02:24 by maboulkh          #+#    #+#             */
// /*   Updated: 2024/01/05 23:03:43 by maboulkh         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "config.hpp"

// class locationContext {
// public:
//     locationContext(ConfigParser &parent, serverContext &server);
//     ~locationContext();
    
//     void setUri();
//     void parseLocationContext();
//     void setLocationData(const string& token);

//     std::string uri;
//     std::string root;
//     std::string index;
//     bool autoindex;
//     int client_max_body_size;
//     std::string cgi;
//     std::string upload_store;
//     std::string upload_pass;
//     std::string upload_pass_args;
//     std::string methods;
//     std::string return_val;
//     std::string auth_basic;
//     std::string auth_basic_user_file;
// private:
//     ConfigParser &parent;
//     serverContext &server;
//     std::vector<configScope>& scopes;
// };

// locationContext::locationContext(ConfigParser &parent, serverContext &server) : parent(parent), server(server), scopes(parent.getScopes()) {
//     uri = "/";
//     root = "";
//     index = "";
//     autoindex = false;
//     client_max_body_size = 0;
//     cgi = "";
//     upload_store = "";
//     upload_pass = "";
//     upload_pass_args = "";
//     methods = "";
//     return_val = "";
//     auth_basic = "";
//     auth_basic_user_file = "";
// }

// locationContext::~locationContext() {}

// void locationContext::setUri() {
//     string token;
//     token = parent.getToken();
//     if (token.empty() && token[0] != '/') {
//         std::cerr << "Error: not a valid URI '" << token << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     uri = token;
// }

// void locationContext::parseLocationContext() {
//     if (scopes.back() != LOCATION || scopes.back() != SERVER) {
//         std::cerr << "Error: Invalid context 'location' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     setUri();
//     if (parent.findBracket("{"))
//         scopes.push_back(LOCATION);
//     else {
//         std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     string token;
//     while (true) {
//         token = parent.getToken();
//         if (token.empty())
//             break;
//         else if (token == "}")
//             break;
//         setLocationData(token);
//     }
//     if (parent.isEndingLine() == false) {
//         std::cerr << "Error: Invalid token '" << parent.getToken() << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     if (scopes.back() != LOCATION) {
//         std::cerr << "Error: Missing closing bracket '}' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     scopes.pop_back();
// }

// void locationContext::setLocationData(const string& token) {
//     if (token == "root") {
//         root = parent.getToken();
//     }
//     else if (token == "index") {
//         index = parent.getToken();
//     }
//     else if (token == "autoindex") {
//         string token = parent.getToken();
//         if (token == "on") {
//             autoindex = true;
//         }
//         else if (token == "off") {
//             autoindex = false;
//         }
//         else {
//             std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//             throw std::exception();
//         }
//     }
//     else if (token == "client_max_body_size") {
//         string token = parent.getToken();
//         client_max_body_size = std::strtol(token);
//     }
//     else if (token == "cgi") {
//         cgi = parent.getToken();
//     }
//     else if (token == "upload_store") {
//         upload_store = parent.getToken();
//     }
//     else if (token == "upload_pass") {
//         upload_pass = parent.getToken();
//     }
//     else if (token == "upload_pass_args") {
//         upload_pass_args = parent.getToken();
//     }
//     else if (token == "methods") {
//         methods = parent.getToken();
//     }
//     else if (token == "return") {
//         return_val = parent.getToken();
//     }
//     else if (token == "auth_basic") {
//         auth_basic = parent.getToken();
//     }
//     else if (token == "auth_basic_user_file") {
//         auth_basic_user_file = parent.getToken();
//     }
//     else {
//         std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     if (parent.getToken() != ";") {
//         std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
// }
