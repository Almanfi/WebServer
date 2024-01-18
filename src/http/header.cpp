/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 22:37:56 by codespace         #+#    #+#             */
/*   Updated: 2024/01/18 01:07:06 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

Header::Header(KeyVal& headers) : headers(headers) {
}

Header::Header(const Header &src) : headers(src.headers) {
}

Header &Header::operator=(const Header &rhs) {
    if (this != &rhs) {}
    return (*this);
}

Header::~Header() {
}

void Header::checkHeadersConflicts () {
    if (HAS_CONTENT_LENGTH(flags) && HAS_TRANSFER_ENCODING(flags))
        throw std::runtime_error("Invalid Headers(transfer encoding with content length)");
    
}

void Header::checkRequiredHeaders() {
    if (!HAS_REQUEST_LINE(flags))
        throw std::runtime_error("Missing Request-Line");
    if (!HAS_HOST(flags))
        throw std::runtime_error("Missing Host");
    if (method == POST) {
        if (!HAS_CONTENT_LENGTH(flags) && !HAS_TRANSFER_ENCODING(flags))
            throw std::runtime_error("Missing Content-Length");
        if (!HAS_CONTENT_TYPE(flags)) // TODO check if content type is really mandatory
            throw std::runtime_error("Missing Content-Type");
    }
}

void Header::validateHeaders() {
    for (KeyVal::const_iterator it = headers.begin(); it != headers.end(); it++) {
        validateHeader(it);
    }
}

void Header::validateHeader(KeyVal::const_iterator& header) {
   if (validationMap.find(header->first) != validationMap.end())
        (this->*validationMap[header->first])(header->second);
    else
        validateOther(header->first, header->second);
}

void Header::validateRequestLine(const string& value) {
    size_t pos = value.find(" ");
    if (pos == string::npos)
        throw std::runtime_error("Invalid Request-Line");
    string methodType = value.substr(0, pos);
    if (std::binary_search(httpAllowedMethods.begin(), httpAllowedMethods.end(), methodType)) {
        if (methodType == "GET")
            method = GET;
        else if (methodType == "POST")
            method = POST;
        else if (methodType == "DELETE")
            method = DELETE;
    }
    else if (std::binary_search(httpOtherMethods.begin(), httpOtherMethods.end(), methodType))
        throw std::runtime_error("Method Not Implemented");
    else
        throw std::runtime_error("Invalid METHOD");
    pos = value.find(" ", pos + 1);
    if (pos == string::npos)
        throw std::runtime_error("Invalid Request-Line");
    string uri = value.substr(0, pos);
    if (uri[0] != '/')
        throw std::runtime_error("Invalid URI");
    string version = value.substr(0, pos + 1);
    if (version != HTTP_VERSION)
        throw std::runtime_error("Invalid HTTP-Version");
    if (HAS_REQUEST_LINE(flags))
        throw std::runtime_error("duplicate Request-Line");
    SET_REQUEST_LINE(flags);
}

void Header::validateHost(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Host");
    if (HAS_HOST(flags))
        throw std::runtime_error("duplicate Host");
    SET_HOST(flags);
}

void Header::validateContentLength(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Content-Length");
    for (size_t i = 0; i < value.length(); i++) {
        if (!std::isdigit(value[i]))
            throw std::runtime_error("Invalid Content-Length");
    }
    stringstream ss(value);
    size_t contentLength;
    ss >> contentLength;
    if (ss.fail())
        throw std::runtime_error("Invalid Content-Length");
    // if (contentLength > MAX_CONTENT_LENGTH)
    //     throw std::runtime_error("Content-Length too large");
    if (HAS_CONTENT_LENGTH(flags))
        throw std::runtime_error("duplicate Content-Length");
    SET_CONTENT_LENGTH(flags);
}

void Header::validateContentType(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Content-Type");
    if (HAS_CONTENT_TYPE(flags))
        throw std::runtime_error("duplicate Content-Type");
    SET_CONTENT_TYPE(flags);
}

void Header::validateTransferEncoding(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Transfer-Encoding");
    if (HAS_TRANSFER_ENCODING(flags))
        throw std::runtime_error("duplicate Transfer-Encoding");
    SET_TRANSFER_ENCODING(flags);
}

void Header::validateConnection(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Connection");
    if (HAS_CONNECTION(flags))
        throw std::runtime_error("duplicate Connection");
    SET_CONNECTION(flags);
}

void Header::validateAcceptCharsets(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Accept-Charsets");
    if (HAS_ACCEPT_CHARSET(flags))
        throw std::runtime_error("duplicate Accept-Charsets");
    SET_ACCEPT_CHARSET(flags);
}

void Header::validateAcceptLanguage(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Accept-Language");
    if (HAS_ACCEPT_LANGUAGE(flags))
        throw std::runtime_error("duplicate Accept-Language");
    SET_ACCEPT_LANGUAGE(flags);
}

void Header::validateAuthorization(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Authorization");
    if (HAS_AUTHORIZATION(flags))
        throw std::runtime_error("duplicate Authorization");
    SET_AUTHORIZATION(flags);
}

void Header::validateAcceptEncoding(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Accept-Encoding");
    if (HAS_ACCEPT_ENCODING(flags))
        throw std::runtime_error("duplicate Accept-Encoding");
    SET_ACCEPT_ENCODING(flags);
}

void Header::validateReferer(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Referer");
    if (HAS_REFERER(flags))
        throw std::runtime_error("duplicate Referer");
    SET_REFERER(flags);
}

void Header::validateUserAgent(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid User-Agent");
    if (HAS_USER_AGENT(flags))
        throw std::runtime_error("duplicate User-Agent");
    SET_USER_AGENT(flags);
}

void Header::validateCookie(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Cookie");
    if (HAS_COOKIE(flags))
        throw std::runtime_error("duplicate Cookie");
    SET_COOKIE(flags);
}

void Header::validateDate(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Date");
    if (HAS_DATE(flags))
        throw std::runtime_error("duplicate Date");
    SET_DATE(flags);
}

void Header::validateServer(const string& value) {
    if (value.empty())
        throw std::runtime_error("Invalid Server");
    if (HAS_SERVER(flags))
        throw std::runtime_error("duplicate Server");
    SET_SERVER(flags);
}

void Header::validateOther(const string& key, const string& value) {
    if (key.empty() || value.empty())
        throw std::runtime_error("Invalid Header");
}

void Header::initHeadersRules() {
    validationMap["Request-Line"] = &Header::validateRequestLine; // TODO ddd key while reading headers
    validationMap["Host"] = &Header::validateHost;
    validationMap["Content-Length"] = &Header::validateContentLength;
    validationMap["Content-Type"] = &Header::validateContentType;
    validationMap["Transfer-Encoding"] = &Header::validateTransferEncoding;
    validationMap["Connection"] = &Header::validateConnection;
    validationMap["Accept-Charsets"] = &Header::validateAcceptCharsets;
    validationMap["Accept-Language"] = &Header::validateAcceptLanguage;
    validationMap["Authorization"] = &Header::validateAuthorization;
    validationMap["Accept-Encoding"] = &Header::validateAcceptEncoding;
    validationMap["Referer"] = &Header::validateReferer;
    validationMap["User-Agent"] = &Header::validateUserAgent;
    validationMap["Cookie"] = &Header::validateCookie;
    validationMap["Date"] = &Header::validateDate;
    validationMap["Server"] = &Header::validateServer;

    httpAllowedMethods.push_back("GET");
    httpAllowedMethods.push_back("POST");
    httpAllowedMethods.push_back("DELETE");
    std::sort(httpAllowedMethods.begin(), httpAllowedMethods.end());

    httpOtherMethods.push_back("PUT");
    httpOtherMethods.push_back("HEAD");
    httpOtherMethods.push_back("OPTIONS");
    httpOtherMethods.push_back("TRACE");
    httpOtherMethods.push_back("CONNECT");
    std::sort(httpOtherMethods.begin(), httpOtherMethods.end());
}
