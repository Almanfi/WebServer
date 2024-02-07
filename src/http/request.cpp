/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:04:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/06 18:10:18 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

// Request::Request() : headerComplete(false) , bodySize(0), contentLength(0), haveChunckSize(false) {
// }

Request::Request(ISBuffer& buffer, IuniqFile& file) :
        buffer(buffer), file(file),
        headerComplete(false) , bodySize(0),
        contentLength(0), haveChunckSize(false) {
}

Request::~Request() {
}

Request::Request(const Request& other) : 
        buffer(other.buffer),
        file(other.file),
        headers(other.headers),
        headerComplete(other.headerComplete),
        bodySize(other.bodySize),
        contentLength(other.contentLength),
        haveChunckSize(other.haveChunckSize){
}

Request& Request::operator=(const Request& other) {
    (void) other;
    throw std::runtime_error("Request::operator=: forbidden");
    // if (this != &other) {
    //     headers = other.headers;
    //     headerComplete = other.headerComplete;
    //     bodySize = other.bodySize;
    //     contentLength = other.contentLength;
    //     haveChunckSize = other.haveChunckSize;
    // }
    // return (*this);
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
            stringstream ss(headers.getHeader(CONTENT_LENGTH));
            ss >> contentLength;
            cout << "end of headers" << endl;
            return ;
        }
        string  line(buff, i);
        size_t  pos = line.find(": ");
        if (pos != string::npos) {
            string  key(line, 0, pos);
            string  value(line, pos + 2);
            headers.insertHeader(key, value);
        }
        else {
            string  key(line, 0, line.find(" "));
            string  value(line, line.find(" ") + 1);
            headers.insertHeader(key, value);
        }
        buff += i + 2;
        size -= i + 2;
        i = 0;
    }
    if (buffer.skip(buff - &buffer) == false) // buffer overflowing with no headers end
        throw std::runtime_error("bad request headers too long");
}

bool Request::hasCRLF(ISBuffer& buffer, size_t pos) {
    if (buffer.find("\r\n", pos) == pos)
        return (true);
    if (static_cast<size_t>(buffer.size()) >= 2)
        throw std::runtime_error("expected \\r\\n at end of chunck1");
    return (false);
}

bool findChunckSize(ISBuffer& buffer, size_t& chunkSize) {
    char *chunk = &buffer;
    size_t i = buffer.find("\r\n", 0);
    if (i == string::npos) {
        if (buffer.freeSpace() < (buffer.capacity() / 100)) // TODO what if buffer capacity is bellow 100
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

void Request::recieveChunkedBody(ISBuffer& buffer, IuniqFile& file) {
    if (haveChunckSize == false) {
        if (findChunckSize(buffer, contentLength) == false)
            return;
        haveChunckSize = true;
    }
    if (contentLength == 0) { // TODO check if this is correct if content length is 0 and we still have a body
        if (hasCRLF(buffer, 0) == false)
            return;
        buffer.clear();
    }
    if (static_cast<size_t>(buffer.size()) <= contentLength) {
        file.write(&buffer, buffer.size());
        // file << buffer;
        bodySize += buffer.size();
        contentLength -= buffer.size();
        buffer.clear();
        return;
    }
    if (hasCRLF(buffer, contentLength) == false)
        return;
    file.write(&buffer, contentLength);
    bodySize += contentLength;
    buffer.skip(contentLength + 2);
    contentLength -= contentLength;
    haveChunckSize = false;
    recieveChunkedBody(buffer, file);
}

void Request::recieveNormalBody(ISBuffer& buffer, IuniqFile& file) {
    size_t size = std::min(static_cast<size_t>(buffer.size()), contentLength);
    if (!buffer.empty())
        file.write(&buffer, size);
    bodySize += size;
    contentLength -= size;
    buffer.clear();
    // if (contentLength == 0) // TODO check if this is correct if content length is 0 and we still have a body
    //     state = WRITE;
}

// bool    Request::parseRequest(SBuffer& buffer, fstream& file) {
//     // ssize_t byte_recieved = 0;
//     // byte_recieved = buffer.recv(fd, 0); // TODO check flags later
//     if (!headerComplete)
//         parseHeaders(buffer);
//    if (headerComplete) {
//     // TODO check content lenght and empty socket after ?
//         if (getHeader(TRANSFER_ENCODING).empty() == false)
//             recieveChunkedBody(buffer, file);
//         else
//             recieveNormalBody(buffer, file);
//         if (contentLength == 0)
//             return (true);
//     }
//     return (false);
// };

bool    Request::parse() {
    // ssize_t byte_recieved = 0;
    // byte_recieved = buffer.recv(fd, 0); // TODO check flags later
    if (!headerComplete)
        parseHeaders();
   if (headerComplete) {
    // TODO check content lenght and empty socket after ?
        if (getHeader(TRANSFER_ENCODING).empty() == false)
            recieveChunkedBody(buffer, file);
        else
            recieveNormalBody(buffer, file);
        if (contentLength == 0)
            return (true);
    }
    return (false);
};

string Request::getHeader(const string& key) {
    return (headers.getHeader(key));
}