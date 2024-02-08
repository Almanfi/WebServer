/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 16:34:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/08 20:16:38 by maboulkh         ###   ########.fr       */
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

enum transferState {
    INCOMPLETE,
    COMPLETE,
};

class ItransferStrategy {
public:
    virtual ~ItransferStrategy() {};
    virtual transferState    transfer(ISBuffer& buffer, IuniqFile& file) = 0;
};

class NormalTransferStrategy : public ItransferStrategy {
    size_t  bodySize;
    size_t  contentLength;
    NormalTransferStrategy(const NormalTransferStrategy& other);
    NormalTransferStrategy& operator=(const NormalTransferStrategy& other);
public:
    NormalTransferStrategy(size_t contentLength);
    ~NormalTransferStrategy();
    transferState    transfer(ISBuffer& buffer, IuniqFile& file);
};

class ChunkedTransferStrategy : public ItransferStrategy {
    size_t  bodySize;
    size_t  contentLength;
    bool    haveChunckSize;
    ChunkedTransferStrategy(const ChunkedTransferStrategy& other);
    ChunkedTransferStrategy& operator=(const ChunkedTransferStrategy& other);
    bool findChunckSize(ISBuffer& buffer, size_t& chunkSize);
    bool hasCRLF(ISBuffer& buffer, size_t pos);
public:
    ChunkedTransferStrategy();
    ~ChunkedTransferStrategy();
    transferState    transfer(ISBuffer& buffer, IuniqFile& file);
};
    

class IRequest {
public:
    virtual ~IRequest() {};
    virtual bool parse() = 0;
    virtual string getHeader(const string& key) = 0;
};

class Request : public IRequest {
public:
    // Request();
    Request(ISBuffer& buffer, IuniqFile& file);
    // Request(const Client& client);
    ~Request();
    Request(const Request& other);
    // bool    parseRequest(SBuffer& buffer, fstream& file);
    bool    parse();
    string  getHeader(const string& key);

private:
    void    setTransferStrategy();
    void    parseHeaders();
    Request& operator=(const Request& other);
    bool hasCRLF(ISBuffer& buffer, size_t pos);
    void recieveChunkedBody(ISBuffer& buffer, IuniqFile& file);
    void recieveNormalBody(ISBuffer& buffer, IuniqFile& file);
    ISBuffer& buffer;
    IuniqFile& file;
    Header  headers;
    bool    headerComplete;
    ItransferStrategy* strategy; 
    // string  body;
    // size_t  bodySize;
    // size_t  contentLength;
    // bool    haveChunckSize;
};

#endif // REQUEST_HPP