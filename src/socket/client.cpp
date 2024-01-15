/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/15 23:32:39 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"


SBuffer::SBuffer() : pos(0), start(0), bytesSize(0), oldBuffer(NULL) {
}

// SBuffer::SBuffer(size_t size) {
// }

SBuffer::SBuffer(const SBuffer& other) : pos(other.pos), start(0), bytesSize(0), oldBuffer(NULL) {
    if (other.oldBuffer){
        size_t size = other.oldBuffer - other.buffer;
        this->oldBuffer = buffer + size;
    }
    std::memcpy(buffer, other.buffer, SBUFFER_SIZE);
}

SBuffer& SBuffer::operator=(const SBuffer& other) {
    if (this != &other) {
        pos = other.pos;
        bytesSize = other.bytesSize;
        start = other.start;
        if (other.oldBuffer) {
            size_t size = other.oldBuffer - other.buffer;
            this->oldBuffer = buffer + size;
        }
        else
            oldBuffer = NULL;
        std::memcpy(buffer, other.buffer, SBUFFER_SIZE);
    }
    return (*this);
}

SBuffer::~SBuffer() {
}

char* SBuffer::operator&() {
    return (buffer);
}

char SBuffer::operator*() {
    return (*buffer);
}

char* SBuffer::operator+(size_t i) {
    return (buffer + i);
}

char* SBuffer::operator-(size_t i) {
    return (buffer - i);
}

char& SBuffer::operator[](size_t i) {
    return (buffer[i]);
}

// char* SBuffer::end() {
//     return (oldBuffer);
// }

char* SBuffer::old() {
    return (oldBuffer);
}

ssize_t SBuffer::begin() {
    return (start);
}

ssize_t SBuffer::size() {
    return (start + bytesSize);
}

bool SBuffer::empty() {
    return (bytesSize == 0);
}

void SBuffer::bzero() {
    std::memset(buffer, 0, SBUFFER_SIZE);
}

ssize_t SBuffer::recv(sock_fd fd, int flags) {
    ssize_t recvSize = ::recv(fd, buffer + size(), SBUFFER_SIZE - size(), flags);
    if (recvSize == -1) {
        perror("recv1");
        throw std::exception();
    }
    bytesSize += recvSize;
    return (recvSize);
    // pos = ::recv(fd, buffer + start, SBUFFER_SIZE - size(), flags);
    // if (pos == -1) {
    //     perror("recv1");
    //     throw std::exception();
    // }
    // return (pos);
}

bool SBuffer::skip(ssize_t newStart) {
    if (newStart > start + bytesSize)
        return (false);
    bytesSize -= newStart - start;
    start = newStart;
    return (true);
}

void SBuffer::clear() {
    start = 0;
    bytesSize = 0;
}

void SBuffer::save(ssize_t from) {
    if (from <= start || from > size()) {
        start = 0;
        oldBuffer = NULL;
        return ;
    }
    if (start == SBUFFER_SIZE || start == 0)
    {
        start = 0;
        oldBuffer = NULL;
    }
    else {
        start = start + from;
        oldBuffer = buffer + from;
        // pos -= from;
    }
}

std::ostream& operator<<(std::ostream& os, SBuffer& buffer) {
    os.write(&buffer, buffer.size());
    os << "|" << buffer.size() << "|" << endl;// for debuging
    return (os);
}

Client::Client(sock_fd fd) : fd(fd), state(NONE) {
    buffer.clear();
}

Client::Client(const Client& other) {
    fd = other.fd;
    state = other.state;
    buffer = other.buffer;
    // std::memcpy(buffer, other.buffer, RECIEVE_MAX_SIZE);
    // buffer_pos = other.buffer_pos;
    // old_buffer = other.old_buffer;
    data = other.data;
}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        fd = other.fd;
        state = other.state;
        buffer = other.buffer;
        // std::memcpy(buffer, other.buffer, RECIEVE_MAX_SIZE);
        // buffer_pos = other.buffer_pos;
        // old_buffer = other.old_buffer;
        data = other.data;
    }
    return (*this);
}

Client::~Client() {
    close(fd);
}

sock_fd Client::getFd() {
    return (fd);
}

ssize_t Client::send() {
    cout << "++++++++++++ send ++++++++++++" << endl;
    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    ssize_t bytes_sent = ::send(fd, response.c_str(), response.size(), 0);
    if (bytes_sent == -1) {
        perror("send");
        throw std::exception();
    }
    state = CLOSE;
    return (bytes_sent);
}

void Client::handle() {

}

ssize_t Client::recieve() {
    cout << "++++++++++++ recieve ++++++++++++" << endl;
    // ssize_t bytes_read = buffer.recv(fd, 0);
    // ssize_t bytes_read = recv(fd, buffer, RECIEVE_MAX_SIZE, 0);
    // ssize_t bytes_read = 0;
    // this->buffer_pos = bytes_read;
    // if (bytes_read == 0) {
    //     std::cout << "Client disconnected" << std::endl;
    //     // delEvent(events[i].data.fd);
    // } else {
    //     cout << "Received request: " << buffer << std::endl;
    // }
    ssize_t bytes_received = request.parseRequest(buffer, fd);
    // if (bytes_received == 0) {
    if (buffer.empty()) { // check later
        state = WRITE;
    }
    return (bytes_received);
    // for (ssize_t i = 0; i < bytes_read; i++)
    //     data += buffer[i];
    // return (bytes_read);
}

void Client::readBuffer() {
    // cout.write(buffer, buffer_pos);
    cout    << "Received request: " << endl
            << buffer << std::endl;
}

cnx_state& Client::getState() {
    return (state);
}