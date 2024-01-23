/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 16:34:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/24 00:45:08 by maboulkh         ###   ########.fr       */
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
    ssize_t parseRequest(SBuffer& buffer, int fd, fstream& file, cnx_state& state);
    string getHeader(const string& key);
    Header  headers;
    string  body;
    // KeyVal headers;
    bool headerComplete;
    size_t  bodySize;
    size_t  contentLength;
    bool finishedChunck;
    bool recieve;

private:
    bool hasCRLF(SBuffer& buffer, size_t pos);
    void recieveChunkedBody(SBuffer& buffer, fstream& file, cnx_state& state);
    void recieveNormalBody(SBuffer& buffer, fstream& file, cnx_state& state);
};

#endif // REQUEST_HPP