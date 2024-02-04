/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdiraa <fdiraa@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 18:57:47 by codespace         #+#    #+#             */
/*   Updated: 2024/02/04 17:04:12 by fdiraa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
#define HEADER_HPP

# include "socket.hpp"

#define HTTP_VERSION "HTTP/1.1"

#define F_HOST                0x00000001
#define F_CONTENT_LENGTH      0x00000002
#define F_CONTENT_TYPE        0x00000004
#define F_TRANSFER_ENCODING   0x00000008
#define F_CONNECTION          0x00000020
#define F_ACCEPT_CHARSET      0x00000040
#define F_ACCEPT_LANGUAGE     0x00000080
#define F_AUTHORIZATION       0x00000100
#define F_ACCEPT_ENCODING     0x00000200
#define F_REFERER             0x00000400
#define F_USER_AGENT          0x00000800
#define F_COOKIE              0x00001000
#define F_DATE                0x00002000
#define F_SERVER              0x00004000
#define F_REQUEST_LINE        0x00010000


#define HAS_REQUEST_LINE(x)         (x & F_REQUEST_LINE)
#define HAS_HOST(x)                 (x & F_HOST)
#define HAS_CONTENT_LENGTH(x)       (x & F_CONTENT_LENGTH)
#define HAS_CONTENT_TYPE(x)         (x & F_CONTENT_TYPE)
#define HAS_TRANSFER_ENCODING(x)    (x & F_TRANSFER_ENCODING)
#define HAS_CONNECTION(x)           (x & F_CONNECTION)
#define HAS_ACCEPT_CHARSET(x)       (x & F_ACCEPT_CHARSET)
#define HAS_ACCEPT_LANGUAGE(x)      (x & F_ACCEPT_LANGUAGE)
#define HAS_AUTHORIZATION(x)        (x & F_AUTHORIZATION)
#define HAS_ACCEPT_ENCODING(x)      (x & F_ACCEPT_ENCODING)
#define HAS_REFERER(x)              (x & F_REFERER)
#define HAS_USER_AGENT(x)           (x & F_USER_AGENT)
#define HAS_COOKIE(x)               (x & F_COOKIE)
#define HAS_DATE(x)                 (x & F_DATE)
#define HAS_SERVER(x)               (x & F_SERVER)

#define SET_REQUEST_LINE(x)         (x |= F_REQUEST_LINE)
#define SET_HOST(x)                 (x |= F_HOST)
#define SET_CONTENT_LENGTH(x)       (x |= F_CONTENT_LENGTH)
#define SET_CONTENT_TYPE(x)         (x |= F_CONTENT_TYPE)
#define SET_TRANSFER_ENCODING(x)    (x |= F_TRANSFER_ENCODING)
#define SET_CONNECTION(x)           (x |= F_CONNECTION)
#define SET_ACCEPT_CHARSET(x)       (x |= F_ACCEPT_CHARSET)
#define SET_ACCEPT_LANGUAGE(x)      (x |= F_ACCEPT_LANGUAGE)
#define SET_AUTHORIZATION(x)        (x |= F_AUTHORIZATION)
#define SET_ACCEPT_ENCODING(x)      (x |= F_ACCEPT_ENCODING)
#define SET_REFERER(x)              (x |= F_REFERER)
#define SET_USER_AGENT(x)           (x |= F_USER_AGENT)
#define SET_COOKIE(x)               (x |= F_COOKIE)
#define SET_DATE(x)                 (x |= F_DATE)
#define SET_SERVER(x)               (x |= F_SERVER)

#define UNSET_CONTENT_LENGTH(x)         (x &= ~F_CONTENT_LENGTH)

typedef enum {
    INVALID,
    GET,
    POST,
    DELETE,
}   t_method;

class Header {
public:
    Header();
    Header(const Header &src);
    Header &operator=(const Header &rhs);
    ~Header();
   
    void validateHeader(const string& key, const string& value);
    void validateHeader(KeyVal::const_iterator& header);
    
    void check();
    void insertHeader(const string& key, const string& value);
    KeyVal& getKeyVal();
    string getHeader(const string& key);
    // void parseHeaders(const string& value);
    // void parseHeader(std::string header);
    static void initHeadersRules();
    KeyVal      keyVal;
    long        flags;
    t_method    method;
    string      uri;
private:
    void validateRequestLine(const string& value);
    void validateHost(const string& value);
    void validateContentLength(const string& value);
    void validateContentType(const string& value);
    void validateTransferEncoding(const string& value);
    void validateConnection(const string& value);
    void validateAcceptCharsets(const string& value);
    void validateAcceptLanguage(const string& value);
    void validateAuthorization(const string& value);
    void validateAcceptEncoding(const string& value);
    void validateReferer(const string& value);
    void validateUserAgent(const string& value);
    void validateCookie(const string& value);
    void validateDate(const string& value);
    void validateServer(const string& value);
    void validateOther(const string& key, const string& value);

    void checkHeadersConflicts();
    void checkRequiredHeaders();
    
    typedef map<string, void (Header::*)(const string&)> HEADER_VALIDATORS;
    static HEADER_VALIDATORS validationMap;
    static vector<string> httpAllowedMethods;
    static vector<string> httpOtherMethods;
};

// #define BUFFER_SIZE 1024
// #define MAX_CONNECTIONS 100

// #define OK 200
// #define CREATED 201
// #define ACCEPTED 202
// #define NO_CONTENT 204
// #define PARTIAL_CONTENT 206
// #define MULTIPLE_CHOICES 300
// #define MOVED_PERMANENTLY 301
// #define FOUND 302
// #define SEE_OTHER 303
// #define NOT_MODIFIED 304
// #define TEMPORARY_REDIRECT 307
// #define BAD_REQUEST 400
// #define UNAUTHORIZED 401
// #define FORBIDDEN 403
// #define NOT_FOUND 404
// #define METHOD_NOT_ALLOWED 405
// #define REQUEST_TIMEOUT 408
// #define LENGTH_REQUIRED 411
// #define PAYLOAD_TOO_LARGE 413
// #define URI_TOO_LONG 414
// #define UNSUPPORTED_MEDIA_TYPE 415
// #define RANGE_NOT_SATISFIABLE 416
// #define EXPECTATION_FAILED 417
// #define IM_A_TEAPOT 418
// #define INTERNAL_SERVER_ERROR 500
// #define NOT_IMPLEMENTED 501
// #define BAD_GATEWAY 502
// #define SERVICE_UNAVAILABLE 503
// #define GATEWAY_TIMEOUT 504

#endif /*HEADER_HPP*/