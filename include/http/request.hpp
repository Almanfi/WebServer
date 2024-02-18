/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 16:34:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/18 19:21:10 by maboulkh         ###   ########.fr       */
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

class IServerSocket;

enum transferState {
    INCOMPLETE,
    COMPLETE,
};

class ITransferStrategy {
public:
    virtual ~ITransferStrategy() {};
    virtual transferState    transfer(ISBuffer& buffer, IUniqFile& file) = 0;
};

class NormalTransferStrategy : public ITransferStrategy {
    IClientConf& config;
    size_t  bodySize;
    size_t  contentLength;
    NormalTransferStrategy(const NormalTransferStrategy& other);
    NormalTransferStrategy& operator=(const NormalTransferStrategy& other);
public:
    NormalTransferStrategy(IClientConf& config, size_t contentLength);
    ~NormalTransferStrategy();
    transferState    transfer(ISBuffer& buffer, IUniqFile& file);
};

class ChunkedTransferStrategy : public ITransferStrategy {
    IClientConf& config;
    size_t  bodySize;
    size_t  contentLength;
    bool    haveChunckSize;
    ChunkedTransferStrategy(const ChunkedTransferStrategy& other);
    ChunkedTransferStrategy& operator=(const ChunkedTransferStrategy& other);
    bool findChunckSize(ISBuffer& buffer, size_t& chunkSize);
    bool hasCRLF(ISBuffer& buffer, size_t pos);
public:
    ChunkedTransferStrategy(IClientConf& config);
    ~ChunkedTransferStrategy();
    transferState    transfer(ISBuffer& buffer, IUniqFile& file);
};
    

class IRequest {
public:
    virtual ~IRequest() {};
    virtual bool parse() = 0;
    virtual string getHeader(const string& key) = 0;
};

class RequestException {
public:
    class BAD_REQUEST : public std::exception {
    public:
        const char* what() const throw();
    };
    class REQUEST_HEADER_FIELDS_TOO_LARGE : public std::exception {
    public:
        const char* what() const throw();
    };
    class NOT_IMPLIMENTED : public std::exception {
    public:
        const char* what() const throw();
    };
    class HTTP_VERSION_NOT_SUPPORTED : public std::exception {
    public:
        const char* what() const throw();
    };
    class LENGTH_REQUIRED : public std::exception {
    public:
        const char* what() const throw();
    };
};

class ISBuffer;

class Request : public IRequest {
public:
    Request(ISBuffer& buffer, IUniqFile& file, IHeader& headers,
            IServerSocket& servSock, IClientConf** configPtr);
    ~Request();
    bool    parse();
    string  getHeader(const string& key);

private:
    void    setTransferStrategy();
    void    parseHeaders();
    void    setConfig();
    Request(const Request& other);
    Request& operator=(const Request& other);
    bool hasCRLF(ISBuffer& buffer, size_t pos);
    ISBuffer&           buffer;
    IUniqFile&          file;
    IHeader&            headers;
    bool                headerComplete;
    bool                haveRequestLine;
    ITransferStrategy*  strategy;
    IServerSocket&      servSock;
    IClientConf**       configPtr;
};

#endif // REQUEST_HPP