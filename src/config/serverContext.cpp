// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   serverContext.cpp                                  :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/01/05 22:53:40 by maboulkh          #+#    #+#             */
// /*   Updated: 2024/01/05 23:06:53 by maboulkh         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "config.hpp"


// class serverContext {
// public:
//     serverContext(ConfigParser &parent);
//     ~serverContext();
    
//     void setServerName();
//     void setLiten();
//     void setErrorPage();
//     void createNewLocation();
//     bool setLocation(const string& token);
    
//     vector<string> server_name;
//     string  listenIp;
//     int     listenPort;
//     std::string error_page;
//     map<string, locationContext> location;

// private:
//     ConfigParser &parent;
// };

// serverContext::serverContext(ConfigParser &parent) : parent(parent) {
//     listenPort = 80;
//     listenIp = "";
//     error_page = "";
//     location.insert(std::pair<string, locationContext>("/", locationContext(parent, *this)));
// }

// serverContext::~serverContext() {}

// void serverContext::setServerName() {
//     string token;
//     while (true) {
//         token = parent.getToken();
//         if (token == ";") {
//             break;
//         }
//         else {
//             server_name.push_back(token);
//         }
//     }
//     if (server_name.empty()) {
//         server_name.push_back("");
//     }
// }

// void serverContext::setLiten() {
//     string token;
//     token = parent.getToken();
//     if (token.find(":") != string::npos) {
//         listenIp = token.substr(0, token.find(":"));
//         listenPort = std::strtol(token.substr(token.find(":") + 1));
//     }
//     else {
//         listenIp = "";
//         listenPort = std::strtol(token);
//     }
//     token = parent.getToken();
//     if (token != ";") {
//         std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
//     // validate port number and ip address
//     // if (listenPort < 0 || listenPort > 65535) {
//     //     std::cerr << "Error: Invalid port number '" << token << "' at line " << parent.getLineNum() << std::endl;
//     //     throw std::exception();
//     // }
// }

// void serverContext::setErrorPage() {
//     string token;
//     token = parent.getToken();
//     if (token != ";") {
//         error_page = token;
//         token = parent.getToken();
//         if (token != ";") {
//             std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//             throw std::exception();
//         }
//     }
//     else {
//         std::cerr << "Error: Invalid token '" << token << "' at line " << parent.getLineNum() << std::endl;
//         throw std::exception();
//     }
// }

// void serverContext::createNewLocation() {
//     locationContext newLocation(parent, *this);
//     newLocation.parseLocationContext();
//     location.insert(std::pair<string, locationContext>(newLocation.root, newLocation));
// }

// bool serverContext::setLocation(const string& token) {
//     if (token == "location") {
//         createNewLocation();
//         return (true);
//     }
//     else {
//         locationContext& loc = location.find("/")->second;
//         loc.setLocationData(token);
//     }
// }

