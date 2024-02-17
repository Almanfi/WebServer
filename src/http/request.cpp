/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:04:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/17 18:55:48 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Request::Request(ISBuffer& buffer, IUniqFile& file, IHeader& headers,
            IServerSocket& servSock, IClientConf* config) :
        buffer(buffer), file(file), headers(headers),
        headerComplete(false), haveRequestLine(false),
        strategy(NULL), servSock(servSock), config(config) {
}

Request::~Request() {
    delete strategy;
}

Request::Request(const Request& other) : 
        buffer(other.buffer),
        file(other.file),
        headers(other.headers),
        headerComplete(other.headerComplete),
        haveRequestLine(other.haveRequestLine),
        strategy(other.strategy),
        servSock(other.servSock),
        config(other.config) {
    throw std::runtime_error("Request::Request: copy constructor forbidden");
}

Request& Request::operator=(const Request& other) {
    (void) other;
    throw std::runtime_error("Request::operator=: forbidden");
}

void Request::setConfig() {
    string configName = headers.getHeader(HOST) + headers.getUri();
    config = &servSock.getLocation(configName);
}

void Request::parseHeaders() {
    cout << "parsing headers" << endl;
    char *buff = &buffer;
    ssize_t size = buffer.size();
    ssize_t i = 0;

    while (i < size) {
        if (!(buff[i] == '\r' && buff[i + 1] == '\n'))
        {
            i++;
            continue ;
        }
        if (i == 0) {
            buffer.skip(buff - &buffer + 2);
            headerComplete = true;
            headers.check();
            setConfig();
            setTransferStrategy();
            cout << "end of headers" << endl;
            return ;
        }
        string  line(buff, i);
        if (haveRequestLine == false) {
            headers.setRequestLine(line);
            haveRequestLine = true;
        }
        else {
            size_t  pos = line.find(": ");
            if (pos == string::npos)
                throw RequestException::BAD_REQUEST();
            string  key(line, 0, pos);
            string  value(line, pos + 2);
            headers.insertHeader(key, value);
        }
        buff += i + 2;
        size -= i + 2;
        i = 0;
    }
    if (buffer.skip(buff - &buffer) == false) // buffer overflowing with no headers end
        throw RequestException::REQUEST_HEADER_FIELDS_TOO_LARGE();
}

bool    Request::parse() {
    if (!headerComplete)
        parseHeaders();
    if (headerComplete) {
        cout << "====== this is for server " << config->getInfo("server_name") << endl;
        if (strategy->transfer(buffer, file) == COMPLETE) {
            return (true);
        }
    }
    return (false);
};

string Request::getHeader(const string& key) {
    return (headers.getHeader(key));
}

void Request::setTransferStrategy() {
    if (headers.getHeader(TRANSFER_ENCODING) == "chunked")
        this->strategy = new ChunkedTransferStrategy(*config);
    if (headers.getMethod() == POST) {
        size_t contentLength;
        string contentLengthStr = headers.getHeader(CONTENT_LENGTH);
        stringstream ss(contentLengthStr);
        ss >> contentLength;
        this->strategy = new NormalTransferStrategy(*config, contentLength);
    }
    else {
        this->strategy = new NormalTransferStrategy(*config, 0);
    }
    cout << "transfer strategy set" << endl;
}


NormalTransferStrategy::NormalTransferStrategy(IClientConf& conf, size_t contentLength) :
            config(conf), bodySize(0), contentLength(contentLength) {
}

NormalTransferStrategy::NormalTransferStrategy(const NormalTransferStrategy& other) : config(other.config) {
    (void) other;
    throw std::runtime_error("NormalTransferStrategy::NormalTransferStrategy: copy constructor forbidden");
}

NormalTransferStrategy& NormalTransferStrategy::operator=(const NormalTransferStrategy& other) {
    (void) other;
    throw std::runtime_error("NormalTransferStrategy::operator=: forbidden");
}

NormalTransferStrategy::~NormalTransferStrategy() {
}

transferState    NormalTransferStrategy::transfer(ISBuffer& buffer, IUniqFile& file) {
    size_t size = std::min(static_cast<size_t>(buffer.size()), contentLength);
    if (!buffer.empty())
        file.write(&buffer, size);
    bodySize += size;
    contentLength -= size;
    buffer.clear();
    return (contentLength == 0 ? COMPLETE : INCOMPLETE);
}

ChunkedTransferStrategy::ChunkedTransferStrategy(IClientConf& conf) :
            config(conf), bodySize(0), contentLength(0), haveChunckSize(false) {
}

ChunkedTransferStrategy::ChunkedTransferStrategy(const ChunkedTransferStrategy& other) : config(other.config) {
    (void) other;
    throw std::runtime_error("ChunkedTransferStrategy::ChunkedTransferStrategy: copy constructor forbidden");
}

ChunkedTransferStrategy& ChunkedTransferStrategy::operator=(const ChunkedTransferStrategy& other) {
    (void) other;
    throw std::runtime_error("ChunkedTransferStrategy::operator=: forbidden");
}

ChunkedTransferStrategy::~ChunkedTransferStrategy() {
}

bool ChunkedTransferStrategy::hasCRLF(ISBuffer& buffer, size_t pos) {
    if (buffer.find("\r\n", pos) == pos)
        return (true);
    if (static_cast<size_t>(buffer.size()) >= 2)
        throw std::runtime_error("expected \\r\\n at end of chunck1");
    return (false);
}

bool ChunkedTransferStrategy::findChunckSize(ISBuffer& buffer, size_t& chunkSize) {
    char *chunk = &buffer;
    size_t i = buffer.find("\r\n", 0);
    if (i == string::npos) {
        if (buffer.freeSpace() < 100) // TODO what if buffer capacity is bellow 100
            buffer.moveDataToStart();
        return (false);
    }
    stringstream ss(string(chunk, 0, i));
    ss >> std::hex >> chunkSize;
    if (ss.fail())
        throw std::runtime_error("fiald to parse chunck size");
    buffer.skip(i + 2);
    cout << "chunck size = " << chunkSize << endl;
    return (true);
}

transferState    ChunkedTransferStrategy::transfer(ISBuffer& buffer, IUniqFile& file) {
    if (haveChunckSize == false) {
        if (findChunckSize(buffer, contentLength) == false)
            return (INCOMPLETE);
        haveChunckSize = true;
    }
    if (contentLength == 0) { // TODO check if this is correct if content length is 0 and we still have a body
        if (hasCRLF(buffer, 0) == false)
            return (INCOMPLETE);
        buffer.clear();
        cout << "++++++++++++++++++ end of chunck ++++++++++++++++" << endl;
        return (COMPLETE);
    }
    if (static_cast<size_t>(buffer.size()) <= contentLength) {
        file.write(&buffer, buffer.size());
        bodySize += buffer.size();
        contentLength -= buffer.size();
        buffer.clear();
        return (INCOMPLETE);
    }
    if (hasCRLF(buffer, contentLength) == false)
        return (INCOMPLETE);
    file.write(&buffer, contentLength);
    buffer.skip(contentLength + 2);
    bodySize += contentLength;
    contentLength -= contentLength;
    if (contentLength == 0)
        haveChunckSize = false;// TODO added this in cas buffer size is not enough to contain the next chunck???
    return (transfer(buffer, file));
}

const char* RequestException::BAD_REQUEST::what() const throw() {
    return ("400");
}

const char* RequestException::REQUEST_HEADER_FIELDS_TOO_LARGE::what() const throw() {
    return ("431");
}

const char* RequestException::NOT_IMPLIMENTED::what() const throw() {
    return ("501");
}

const char* RequestException::HTTP_VERSION_NOT_SUPPORTED::what() const throw() {
    return ("505");
}

const char* RequestException::LENGTH_REQUIRED::what() const throw() {
    return ("411");
}
