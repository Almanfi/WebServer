/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdiraa <fdiraa@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 16:34:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/05 19:22:14 by fdiraa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "socket.hpp"

typedef map<string, string>  KeyVal;

//HTTP HEADERS
# define CONTENT_LENGTH "Content-Length"
# define CONTENT_TYPE "Content-Type"
# define HOST "Host"
# define CONNECTION "Connection"
# define DATE "Date"
# define SERVER "Server"
# define LOCATION "Location"
# define TRANSFER_ENCODING "Transfer-Encoding"

class SBuffer;

class Request {
public:
    Request();
    // Request(const Client& client);
    ~Request();
    Request(const Request& other);
    Request& operator=(const Request& other);
    void    parseHeaders(SBuffer& buffer);
    bool    parseRequest(SBuffer& buffer, fstream& file);
    string  getHeader(const string& key);
    Header  headers;
    string  body;

private:
    bool hasCRLF(SBuffer& buffer, size_t pos);
    void recieveChunkedBody(SBuffer& buffer, fstream& file);
    void recieveNormalBody(SBuffer& buffer, fstream& file);
    bool    headerComplete;
    size_t  bodySize;
    size_t  contentLength;
    bool    haveChunckSize;
    // server 
    // method 
    
};

#endif // REQUEST_HPP