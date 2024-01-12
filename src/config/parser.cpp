/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:24 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/12 16:24:48 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/parser.hpp"

Parser::Parser(const std::string& filePath) : lineNumber(0), linePos(0) {
    configFile.open(filePath.c_str(), std::ios::in);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config file " + filePath);
    }
}

Parser::~Parser() {
    configFile.close();
}

// bool Parser::findBracket(const std::string& bracket) {
//     std::string token;
//     while (true) {
//         token = getToken();
//         if (token.empty())
//             break;
//         if (token == bracket) {
//             return true;
//         }
//         else {
//             stringstream ss;
//             ss << getLineNum();
//             throw std::runtime_error("Error: Invalid token (need an opening bracket) '"
//                                         + token + "' at line " + ss.str());
//         }
//     }
//     return false;
// }

bool Parser::is_space(const char c) {
    if (c == ' ' || c == '\t')
        return true;
    return false;
}

bool Parser::is_delimitter(const char c) {
    if (c == '{' || c == '}' || c == ';')
        return true;
    return false;
}

// bool Parser::isEndingLine() {
//     while (is_space(line[linePos]))
//         linePos++;
//     if (line[linePos] == '#' || linePos >= line.length()) {
//         if (!std::getline(configFile, line))
//             line = "";
//         lineNumber++;
//         cout << "line " << lineNumber << ": " << line << endl;
//         linePos = 0;
//         return true;
//     }
//     return false;
// }

std::string Parser::getToken() {
    size_t& i = linePos;
    if (i >= line.length()) {
        if (!std::getline(configFile, line)) {
            return string("");
        }
        lineNumber++;
        // cout << "line " << lineNumber << ": " << line << endl;
        i = 0;
    }
    while (is_space(line[i]))
        i++;
    if (line[i] == '#' || i >= line.length()) {
        i = line.length();
        // std::cout << "Comment : skip line!" << std::endl;
        return getToken();
    }
    if (is_delimitter(line[i])) {
        i++;
        std::string delimitter = line.substr(i - 1, 1);
        if (delimitter == ";") {
            while (is_space(line[i]))
                i++;
            // if (line[i] == '#' || i >= line.length()) {
            //     i = line.length();
            //     std::cout << "Comment : skip line after ';'!" << std::endl;
            // }
            // else if (line[i] == '}' || line[i] == ';')
            //     ;
            // else {
            //     std::cerr << "Error: found token after ';' " << " at line " << lineNumber << std::endl;
            //     throw std::exception();
            // }
        }
        return delimitter;
    }
    size_t start = i;
    while (!is_space(line[i]) && !is_delimitter(line[i]) && line[i] != '\0')
        i++;
    return line.substr(start, i - start);
}

int Parser::getLineNum() {
    return lineNumber;
}

std::vector<configScope>& Parser::getScopes() {
    return scopes;
}
