/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elasce <elasce@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/26 18:14:15 by elasce           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Client::Client(sock_fd fd, ServerSocket& servSock) :
                                                    fd(fd),
                                                    state(NONE),
                                                    uuid(),
                                                    servSock(servSock) {
    while (access((string("/tmp/") + uuid.getStr()).c_str(), F_OK) == 0) {
        uuid.regen();
    }
    createFile();
    buffer.clear();
}

Client::Client(const Client& other) : 
                                    fd(other.fd),
                                    state(other.state),
                                    uuid(other.uuid),
                                    buffer(other.buffer),
                                    servSock(other.servSock),
                                    request(other.request) {           
}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        fd = other.fd;
        state = other.state;
        uuid = other.uuid;
        buffer = other.buffer;
        request = other.request;
    }
    return (*this);
}

Client::~Client() {
    if (file.is_open())
        file.close();
    // remove((string("./tmp/") + uuid.getStr()).c_str()); // TODO uncomment
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
    openFile();
    ssize_t bytes_received = buffer.recv(fd, 0); // TODO check flags later
    if (request.parseRequest(buffer, file) == true)
        state = WRITE;
    file.close();
    return (bytes_received);
}

const cnx_state& Client::handleState() {
    if (state == NONE)
        state = READ;
    switch (state) {
        case READ:
            if (recieve() == 0)
                state = CLOSE;
            break ;
        case WRITE:
            send();
            break ;
        case CLOSE:
            cout << "++++++++++++ closing ++++++++++++" << endl;
            break ;
        default:
            throw std::runtime_error("Client::handle: invalid state");
    }
    return (state);
}

void Client::readBuffer() {
    cout    << "Received request: " << endl
            << buffer << std::endl;
}

// cnx_state& Client::getState() {
//     return (state);
// }

Server& Client::getServer() {
    return (*servSock.getServers()[0]);
}

void Client::createFile() {
    file.open((string("./tmp/") + uuid.getStr()).c_str(), std::ios::in | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        perror("open");
        throw std::exception();
    }
    file.close();
}

void Client::openFile() {
    file.open((string("./tmp/") + uuid.getStr()).c_str(), std::ios::in | std::ios::out | std::ios::app);
    if (!file.is_open()) {
        perror("open");
        throw std::exception();
    }
}

const UUID& Client::getUUID() {
    return (uuid);
}
