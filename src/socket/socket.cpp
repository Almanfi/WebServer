/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 12:36:29 by bamrouch          #+#    #+#             */
/*   Updated: 2024/01/12 16:19:41 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Socket::Socket(const string& ip, const string& port) {
    addrinfo *res;
    addrinfo hints;
    std::memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // If node is not NULL, then the AI_PASSIVE flag is ignored.
    int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        throw std::exception();
    }
    sockid = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockid == -1) {
        perror("socket");
        throw std::exception();
    }
    int opt = 1;
    if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        throw std::exception();
    }
    sockBind(res);
    sockListen();
    freeaddrinfo(res);
}

Socket::Socket(Server& serv) {
    addrinfo *res;
    addrinfo hints;
    std::memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // If node is not NULL, then the AI_PASSIVE flag is ignored.
    int status = getaddrinfo(serv.getInfo("host").c_str(), serv.getInfo("port").c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        throw std::exception();
    }
    sockid = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockid == -1) {
        perror("socket");
        throw std::exception();
    }
    int opt = 1;
    if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        throw std::exception();
    }
    sockBind(res);
    sockListen();
    freeaddrinfo(res);
}

Socket::~Socket() {
    close(sockid);
}

void Socket::sockBind(addrinfo *res) {
    if (bind(sockid, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        throw std::exception();
    }
}

void Socket::sockListen() {
    if (listen(sockid, 10) == -1) {
        perror("listen");
        throw std::exception();
    }
}

int Socket::sockAccept() {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(sockid, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("accept");
        throw std::exception();
    }
    return (client_fd);
}

void Socket::sockClose() {
    close(sockid);
}

int Socket::getSockid() {
    return sockid;
}

void Socket::setServer(Server* server) {
    this->server = server;
}

Epoll::Epoll(Config& config) {
    (void) config;
    deque<Server>& servers =  config.getServers();
    epollfd = epoll_create(10);// test with 0 later
    if (epollfd < 0) {
        perror("epoll_create");
        throw std::exception();
    }
    for (size_t i = 0; i < servers.size(); i++) {
        servSocket.push_back(new Socket(servers[i]));
        std::memset(&event, 0, sizeof(event));
        event.data.fd = servSocket.back()->getSockid();
        event.events = EPOLLIN;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servSocket.back()->getSockid(), &event) == -1) {
            perror("epoll_ctl");
            throw std::exception();
        }
    }
    cout << "number of servers listening: " << servSocket.size() << endl;
}

Epoll::Epoll(Socket* socket) : socket(socket) {
    epollfd = epoll_create(10);// test with 0 later
    if (epollfd < 0) {
        perror("epoll_create");
        throw std::exception();
    }
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket->getSockid();
    event.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket->getSockid(), &event) == -1) {
        perror("epoll_ctl");
        throw std::exception();
    }
}

Epoll::~Epoll() {
    close(epollfd);
    for (size_t i = 0; i < servSocket.size(); i++) {
        delete &servSocket[i];
    }
}

void Epoll::loop() {
    // int sockfd = socket->getSockid();
    while (true) {
            // cout << "client size: " << fds.size() << endl;
            int n = epoll_wait(epollfd, events, 10, -1);
            if (n == -1) {
                perror("epoll_wait");
                throw std::exception();
            }
            for (int i = 0; i < n; i++) {
                size_t index;
                for (index = 0; index < servSocket.size(); index++) {
                    if (events[i].data.fd == servSocket[index]->getSockid()) {
                        socket = servSocket[index];
                        break;
                    }
                }
                if (index < servSocket.size()) {
                    int client_fd = socket->sockAccept();
                    fds.push_back(client_fd);
                    std::cout << "New client connected" << std::endl;
                    event.data.fd = client_fd;
                    event.events = EPOLLIN;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                        perror("epoll_ctl");
                        throw std::exception();
                    }
                } else {
                    char buffer[1024];
                    ssize_t bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                    if (bytes_read == -1) {
                        perror("recv");
                        throw std::exception();
                    } else if (bytes_read == 0) {
                        std::cout << "Client disconnected" << std::endl;
                        close(events[i].data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        for (size_t j = 0; j < fds.size(); j++) {
                            if (fds[j] == events[i].data.fd) {
                                fds.erase(fds.begin() + j);
                                break;
                            }
                        }
                    } else {
                        // Process the received data
                        std::string request(buffer, bytes_read);
                        std::cout << "Received request: " << request << std::endl;
                        // Close the connection after finishing reading the request
                        close(events[i].data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        for (size_t j = 0; j < fds.size(); j++) {
                            if (fds[j] == events[i].data.fd) {
                                fds.erase(fds.begin() + j);
                                break;
                            }
                        }
                        std::exit(0);
                    }
                }
            }
        }
}