/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/19 21:38:24 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

SBuffer::SBuffer() : start(0), count(0) {
}

SBuffer::SBuffer(const SBuffer& other) : start(other.start), count(other.count) {
    std::memcpy(buffer + start, other.buffer + start, count); // TODO is this safe? debug
}

SBuffer& SBuffer::operator=(const SBuffer& other) {
    if (this != &other) {
        count = other.count;
        start = other.start;
        std::memcpy(buffer + start, other.buffer + start, count);
    }
    return (*this);
}

SBuffer::~SBuffer() {
}

char* SBuffer::operator&() {
    return (buffer + start);
}

char& SBuffer::operator*() {
    return (*(buffer + start));
}

char* SBuffer::operator+(size_t i) {
    return (buffer + start + i);
}

char* SBuffer::operator-(size_t i) {
    return (buffer + start - i);
}

char& SBuffer::operator[](size_t i) {
    return (buffer[start + i]);
}

ssize_t SBuffer::begin() {
    return (start);
}

ssize_t SBuffer::size() {
    return (count);
}

ssize_t SBuffer::end() {
    return (start + count);
}

bool SBuffer::empty() {
    return (count == 0);
}

void SBuffer::bzero() {
    std::memset(buffer, 0, SBUFFER_SIZE);
}

ssize_t SBuffer::recv(sock_fd fd, int flags) {
    // if (end() == SBUFFER_SIZE) // TODO is this necesary? might delete data
    //     clear(); //
    ssize_t recvSize = ::recv(fd, buffer + end(), SBUFFER_SIZE - end(), flags);
    if (recvSize == -1) {
        perror("recv1");
        throw std::exception();
    }
    count += recvSize;
    return (recvSize);
}

bool SBuffer::skip(ssize_t offset) {
    if (offset > count)
        return (false);
    count -= offset;
    start += offset;
    return (true);
}

void SBuffer::clear() {
    start = 0;
    count = 0;
}

std::ostream& operator<<(std::ostream& os, SBuffer& buffer) {
    os.write(&buffer, buffer.size());
    os << "|" << buffer.size() << "|" << endl;// for debuging
    return (os);
}

Client::Client(sock_fd fd, ServerSocket& servSock) : fd(fd), state(NONE), servSock(servSock) {
    buffer.clear();
    data.clear();
}

Client::Client(const Client& other) : 
                                    fd(other.fd),
                                    state(other.state),
                                    buffer(other.buffer),
                                    data(other.data),
                                    servSock(other.servSock){
}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        fd = other.fd;
        state = other.state;
        buffer = other.buffer;
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
    // std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    std::string response = "HTTP/1.1 300\r\nLocation: https://www.google.com/\r\n\r\n";
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
    ssize_t bytes_received = request.parseRequest(buffer, fd);
    if (buffer.empty() && request.headerComplete) // TODO check later
        state = WRITE;
    return (bytes_received);
}

void Client::readBuffer() {
    cout    << "Received request: " << endl
            << buffer << std::endl;
}

cnx_state& Client::getState() {
    return (state);
}