/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:04:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/23 16:13:04 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Request::Request() : headerComplete(false) , bodySize(0), contentLength(0) {
}

Request::~Request() {
}

Request::Request(const Request& other) : 
        headers(other.headers),
        body(other.body),
        headerComplete(other.headerComplete),
        bodySize(other.bodySize){
}

Request& Request::operator=(const Request& other) {
    if (this != &other) {
        headers = other.headers;
        body = other.body;
        headerComplete = other.headerComplete;
        bodySize = other.bodySize;
    }
    return (*this);
}

void Request::parseHeaders(SBuffer& buffer) {
    cout << "parsing headers" << endl;
    char *buff = &buffer;
    // cout << "reding buffer : " << buffer.size()<< endl;
    cout << "buffer : " << buffer << endl;
    ssize_t size = buffer.size();
    ssize_t i = 0;
    
    while (i < size) {
        if (!(buff[i] == '\r' && buff[i + 1] == '\n'))
        {
            i++;
            continue ;
        }
        cout << "line is  = " << string(buff, i) << endl;
        if (i == 0) {
            cout << "got here" << endl;
            buffer.skip(buff - &buffer + 2);
            // buffer.save(buff - &buffer + 2);
            headerComplete = true;
            headers.check();
            stringstream ss(headers.getHeader(CONTENT_LENGTH));
            ss >> contentLength;
            body.empty();
            cout << "end of headers" << endl;
            return ;
        }
        string  line(buff, i);
        size_t  pos = line.find(": ");
        if (pos != string::npos) {
            string  key(line, 0, pos);
            string  value(line, pos + 2);
            headers.insertHeader(key, value);
            // headers.insert(std::make_pair(key, value));
        }
        else {
            string  key(line, 0, line.find(" "));
            string  value(line, line.find(" ") + 1);
            headers.insertHeader(key, value);
            // headers.insert(std::make_pair(key, value));
        }
        buff += i + 2;
        size -= i + 2;
        i = 0;
    }
    if (buffer.skip(buff - &buffer) == false) // buffer overflowing with no headers end
        throw std::runtime_error("bad request headers too long");
}

ssize_t    Request::parseRequest(SBuffer& buffer ,int fd, fstream& file, cnx_state& state) {
    ssize_t byte_recieved = buffer.recv(fd, 0); // check flags later
    if (!headerComplete)
        parseHeaders(buffer);
   if (headerComplete) {
        if (!buffer.empty())
            file << buffer;
        bodySize += buffer.size();
        buffer.clear();
        if (bodySize >= contentLength)
            state = WRITE;
    }
    return (byte_recieved);
};

string Request::getHeader(const string& key) {
    return (headers.getHeader(key));
}