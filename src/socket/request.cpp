/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:04:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/23 00:19:06 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Request::Request() : headerComplete(false) , bodySize(0){
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

ssize_t    Request::parseRequest(SBuffer& buffer ,int fd, fstream& file, size_t contentLength) {
    ssize_t byte_recieved = buffer.recv(fd, 0); // check flags later
    if (!headerComplete)
        parseHeaders(buffer);
   if (headerComplete) {
        headers.check();
        // cout << "body size : " << body.size() << endl; // debuging
        if (!buffer.empty())
            file << buffer;
        bodySize += buffer.size();
            // body.append(&buffer, buffer.size());
        // cout << "body size : " << body.size() << endl;
        buffer.clear();
        if (contentLength && bodySize >= contentLength) {
            cout << "body size : " << bodySize << endl;
            cout << "content length : " << contentLength << endl;
            cout << "end of body" << endl;
            return (-1);
        }
        // buffer.save(0); // debuging
    }
    return (byte_recieved);
};
