/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 20:03:18 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 20:19:41 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exception.hpp"

string ConfigException::getLineNum() {
    stringstream ss;
    ss << Parser::getInstance().getLineNum();
    return ss.str();
}

ConfigException::DIRECT_NOT_VALID::DIRECT_NOT_VALID(const string& directive) {
    msg = "Error: directive `" + directive
        + "` not valid at line " + getLineNum();
}

ConfigException::DIRECT_NOT_VALID::~DIRECT_NOT_VALID() throw() {}

const char* ConfigException::DIRECT_NOT_VALID::what() const throw() {
    return msg.c_str();
}

ConfigException::MISSING_BRACKET::MISSING_BRACKET(const string& bracket) {
    msg = "Error: Missing ";
    if (bracket == "{")
        msg += "opening ";
    else if (bracket == "}")
        msg += "closing ";
    msg += "bracket at line " + getLineNum();
}

ConfigException::MISSING_BRACKET::~MISSING_BRACKET() throw() {}

const char* ConfigException::MISSING_BRACKET::what() const throw() {
    return msg.c_str();
}


ConfigException::DIRECT_ALREADY_SET::DIRECT_ALREADY_SET(const string& directive) {
    msg = "Error: directive `" + directive
        + "` already set at line " + getLineNum();
}

ConfigException::DIRECT_ALREADY_SET::~DIRECT_ALREADY_SET() throw() {}

const char* ConfigException::DIRECT_ALREADY_SET::what() const throw() {
    return msg.c_str();
}


ConfigException::TOO_MANY_ARGS::TOO_MANY_ARGS(const string& directive) {
    msg = "Error: too many arguments for directive `"
        + directive + "` at line " + getLineNum();
}

ConfigException::TOO_MANY_ARGS::~TOO_MANY_ARGS() throw() {}

const char* ConfigException::TOO_MANY_ARGS::what() const throw() {
    return msg.c_str();
}


ServerException::NOT_VALID_PORT::NOT_VALID_PORT(const string& port) {
    msg = "Error: port `" + port
        + "` not valid at line " + getLineNum();
}

ServerException::NOT_VALID_PORT::~NOT_VALID_PORT() throw() {}

const char* ServerException::NOT_VALID_PORT::what() const throw() {
    return msg.c_str();
}

ServerException::LISTEN_NOT_FOUND::LISTEN_NOT_FOUND() {
    msg = "Error: directive `listen` not found at line "
        + getLineNum();
}

ServerException::LISTEN_NOT_FOUND::~LISTEN_NOT_FOUND() throw() {}

const char* ServerException::LISTEN_NOT_FOUND::what() const throw() {
    return msg.c_str();
}

ServerException::SERVER_NAME_NOT_FOUND::SERVER_NAME_NOT_FOUND(const string& serverName) {
    msg = "Error: server `" + serverName + "` not found";
}

ServerException::SERVER_NAME_NOT_FOUND::~SERVER_NAME_NOT_FOUND() throw() {}

const char* ServerException::SERVER_NAME_NOT_FOUND::what() const throw() {
    return msg.c_str();
}

ServerException::MISSING_LOCATION_URI::MISSING_LOCATION_URI() {
    msg = "Error: missing location URI at line "
        + getLineNum();
}

ServerException::MISSING_LOCATION_URI::~MISSING_LOCATION_URI() throw() {}

const char* ServerException::MISSING_LOCATION_URI::what() const throw() {
    return msg.c_str();
}

ServerException::INVALID_LOCATION_URI::INVALID_LOCATION_URI(const string& uri) {
    msg = "Error: location URI `" + uri
        + "` not valid at line " + getLineNum();
}

ServerException::INVALID_LOCATION_URI::~INVALID_LOCATION_URI() throw() {}

const char* ServerException::INVALID_LOCATION_URI::what() const throw() {
    return msg.c_str();
}

LocationException::INVALID_METHOD::INVALID_METHOD(const string& method) {
    msg = "Error: method `" + method
        + "` is not supported at line " + getLineNum();
}

LocationException::INVALID_METHOD::~INVALID_METHOD() throw() {}

const char* LocationException::INVALID_METHOD::what() const throw() {
    return msg.c_str();
}

LocationException::INVALID_ROOT::INVALID_ROOT(const string& root) {
    msg = "Error: root `" + root
        + "` not valid at line " + getLineNum();
}

LocationException::INVALID_ROOT::~INVALID_ROOT() throw() {}

const char* LocationException::INVALID_ROOT::what() const throw() {
    return msg.c_str();
}

LocationException::INVALID_VALUE::INVALID_VALUE(const string& key, const string& value) {
    msg = "Error: value `" + value + "` not valid for the "
        + key  + " directive at line " + getLineNum();
}

LocationException::INVALID_VALUE::~INVALID_VALUE() throw() {}

const char* LocationException::INVALID_VALUE::what() const throw() {
    return msg.c_str();
}

LocationException::INVALID_PATH::INVALID_PATH(const string& path) {
    msg = "Error: path `" + path
        + "` not valid at line " + getLineNum();
}

LocationException::INVALID_PATH::~INVALID_PATH() throw() {}

const char* LocationException::INVALID_PATH::what() const throw() {
    return msg.c_str();
}

LocationException::MISSING_CGI_PATH::MISSING_CGI_PATH(const string& cgi_uri) {
    msg = "Error: cgi_path for `" + cgi_uri
        + "` is missing at line " + getLineNum();
}

LocationException::MISSING_CGI_PATH::~MISSING_CGI_PATH() throw() {}

const char* LocationException::MISSING_CGI_PATH::what() const throw() {
    return msg.c_str();
}
