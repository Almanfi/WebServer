/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:24 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 03:46:24 by maboulkh         ###   ########.fr       */
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

std::string Parser::getToken() {
    size_t& i = linePos;
    if (i >= line.length()) {
        if (!std::getline(configFile, line)) {
            return string("");
        }
        lineNumber++;
        i = 0;
    }
    while (is_space(line[i]))
        i++;
    if (line[i] == '#' || i >= line.length()) {
        i = line.length();
        return getToken();
    }
    if (is_delimitter(line[i])) {
        i++;
        std::string delimitter = line.substr(i - 1, 1);
        if (delimitter == ";") {
            while (is_space(line[i]))
                i++;
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
