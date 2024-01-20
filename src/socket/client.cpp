/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 04:34:39 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

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
    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    // std::string response = "HTTP/1.1 300\r\nLocation: https://www.google.com/\r\n\r\n";
    ssize_t bytes_sent = ::send(fd, response.c_str(), response.size(), 0);
    if (bytes_sent == -1) {
        perror("send");
        throw std::exception();
    }
    state = CLOSE;
    return (bytes_sent);
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

Server& Client::getServer() {
    return (*servSock.getServers()[0]);
}