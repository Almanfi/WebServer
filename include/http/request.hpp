/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elasce <elasce@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 16:34:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/26 18:10:25 by elasce           ###   ########.fr       */
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

private:
    bool hasCRLF(SBuffer& buffer, size_t pos);
    void recieveChunkedBody(SBuffer& buffer, fstream& file);
    void recieveNormalBody(SBuffer& buffer, fstream& file);
    Header  headers;
    string  body;
    bool    headerComplete;
    size_t  bodySize;
    size_t  contentLength;
    bool    haveChunckSize;
};

#endif // REQUEST_HPP