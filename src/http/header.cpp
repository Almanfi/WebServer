/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 22:37:56 by codespace         #+#    #+#             */
/*   Updated: 2024/02/14 18:05:31 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Header::Header() : flags(0), method(INVALID) {
}

Header::Header(const Header &src) : keyVal(src.keyVal), flags(src.flags) {
}

Header &Header::operator=(const Header &rhs) {
    if (this != &rhs) {}
    return (*this);
}

Header::~Header() {
}

map<string, void (Header::*)(const string&)> Header::validationMap;
map<string, t_method> Header::httpMethods;

void Header::checkHeadersConflicts () {
    if (HAS_CONTENT_LENGTH(flags) && HAS_TRANSFER_ENCODING(flags)) {
        keyVal.erase(CONTENT_LENGTH);
        UNSET_CONTENT_LENGTH(flags);
    }
}

void Header::checkRequiredHeaders() {
    if (!HAS_REQUEST_LINE(flags))
        throw RequestException::BAD_REQUEST();
    // if (method == INVALID)
    //     throw std::runtime_error("Missing Request-Line");
    if (!HAS_HOST(flags))
        throw RequestException::BAD_REQUEST();
    if (method == POST) {
        if (!HAS_CONTENT_LENGTH(flags) && !HAS_TRANSFER_ENCODING(flags))
            throw RequestException::LENGTH_REQUIRED();
        // if (!HAS_CONTENT_TYPE(flags)) // TODO check if content type is really mandatory
        //     throw std::runtime_error("Missing Content-Type");
    }
}

void Header::check() {
    checkHeadersConflicts();
    checkRequiredHeaders();
}

void Header::insertHeader(const string& key, const string& value) {
    validateHeader(key, value);
    keyVal.insert(std::make_pair(key, value));
}

KeyVal& Header::getKeyVal() {
    return (keyVal);
}

string Header::getHeader(const string& key) {
    if (keyVal.find(key) == keyVal.end())
        return ("");
    return (keyVal[key]);
}

t_method Header::getMethod() const {
    return (method);
}

const string& Header::getUri() const {
    return (uri);
}

void Header::validateHeader(const string& key, const string& value) {
    if (validationMap.find(key) != validationMap.end())
        (this->*validationMap[key])(value);
    else
        validateOther(key, value);
}

void Header::validateHeader(KeyVal::const_iterator& header) {
   if (validationMap.find(header->first) != validationMap.end())
        (this->*validationMap[header->first])(header->second);
    else
        validateOther(header->first, header->second);
}

void Header::setRequestLine(const string& value) {
    size_t pos = value.find(" ");
    if (pos == string::npos)
        throw RequestException::BAD_REQUEST();
    string methodType = value.substr(0, pos);
    if (httpMethods.find(methodType) == httpMethods.end())
        throw RequestException::NOT_IMPLIMENTED();
    method = httpMethods[methodType];
    size_t start = pos + 1;
    pos = value.find(" ", start);
    if (pos == string::npos)
        throw RequestException::BAD_REQUEST();
   uri = value.substr(start , pos - start);
    if (uri[0] != '/')
        throw RequestException::BAD_REQUEST();
    if (uri.find("..") != string::npos)
        throw RequestException::BAD_REQUEST();
    string version = value.substr(pos + 1);
    if (version != HTTP_VERSION)
        throw RequestException::HTTP_VERSION_NOT_SUPPORTED();
    if (HAS_REQUEST_LINE(flags))
        throw RequestException::BAD_REQUEST();
    SET_REQUEST_LINE(flags);
}

void Header::validateHost(const string& value) {
    if (value.empty())
        throw RequestException::BAD_REQUEST();
    if (HAS_HOST(flags))
        throw RequestException::BAD_REQUEST();
    SET_HOST(flags);
}

void Header::validateContentLength(const string& value) {
    if (value.empty())
        throw RequestException::BAD_REQUEST();
    for (size_t i = 0; i < value.length(); i++) {
        if (!std::isdigit(value[i]))
            throw RequestException::BAD_REQUEST();
    }
    stringstream ss(value);
    size_t contentLength;
    ss >> contentLength;
    if (ss.fail())
        throw RequestException::BAD_REQUEST();
    // if (contentLength > MAX_CONTENT_LENGTH) // TODO add limit checking here
    //     throw std::runtime_error("Content-Length too large");
    if (HAS_CONTENT_LENGTH(flags))
        throw RequestException::BAD_REQUEST();
    SET_CONTENT_LENGTH(flags);
}

void Header::validateContentType(const string& value) {
    if (value.empty())
        throw RequestException::BAD_REQUEST();
    if (HAS_CONTENT_TYPE(flags))
        throw RequestException::BAD_REQUEST();
    SET_CONTENT_TYPE(flags);
}

void Header::validateTransferEncoding(const string& value) {
    if (value != "chunked")
        throw RequestException::NOT_IMPLIMENTED();
    if (HAS_TRANSFER_ENCODING(flags))
        throw RequestException::BAD_REQUEST();
    SET_TRANSFER_ENCODING(flags);
}

void Header::validateDate(const string& value) {
    if (value.empty())
        throw RequestException::BAD_REQUEST();
    if (HAS_DATE(flags))
        throw RequestException::BAD_REQUEST();
    SET_DATE(flags);
}


void Header::validateOther(const string& key, const string& value) {
    (void) value;
    if (key.empty())
        throw RequestException::BAD_REQUEST();
}

void Header::initHeadersRules() {
    validationMap["Host"] = &Header::validateHost;
    validationMap["Date"] = &Header::validateDate;
    validationMap["Content-Length"] = &Header::validateContentLength;
    validationMap["Content-Type"] = &Header::validateContentType;
    validationMap["Transfer-Encoding"] = &Header::validateTransferEncoding;

    httpMethods["GET"] = GET;
    httpMethods["POST"] = POST;
    httpMethods["DELETE"] = DELETE;
    httpMethods["PUT"] = PUT;
    httpMethods["HEAD"] = HEAD;
    httpMethods["OPTIONS"] = OPTIONS;
    httpMethods["CONNECT"] = CONNECT;
    httpMethods["TRACE"] = TRACE;
    httpMethods["PATCH"] = PATCH;
    for (map<string, t_method>::iterator it = httpMethods.begin();
        it != httpMethods.end(); it++) {
            methodToString[it->second] = it->first;
    }
}

string& Header::getMethodString(t_method method) {
    return (methodToString[method]);
}
