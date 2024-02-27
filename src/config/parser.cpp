/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:24 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 23:20:17 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/parser.hpp"

Parser Parser::instance;

Parser::Parser() : lineNumber(0), linePos(0) {
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

void Parser::init(const std::string& filePath) {
    if (instance.isInitialized) {
        throw std::runtime_error("Parser already initialized");
    }
    instance.configFile.open(filePath.c_str(), std::ios::in);
    if (!instance.configFile.is_open()) {
        throw std::runtime_error("Failed to open config file " + filePath);
    }
    instance.isInitialized = true;
}

Parser& Parser::getInstance() {
    if (!instance.isInitialized) {
        throw std::runtime_error("Parser not initialized");
    }
    return instance;
}

void Parser::destroy() {
    if (!instance.isInitialized) {
        throw std::runtime_error("Parser not initialized");
    }
    instance.configFile.close();
}

std::string Parser::getTok() {
    return instance.getToken();
}