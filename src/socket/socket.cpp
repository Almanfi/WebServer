/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 12:36:29 by bamrouch          #+#    #+#             */
/*   Updated: 2024/01/19 21:48:33 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Socket::Socket() {
}

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

Socket::Socket(const Server& serv) {
    addrinfo hints;
    std::memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // If node is not NULL, then the AI_PASSIVE flag is ignored.
    int status = getaddrinfo(serv.getInfo(S_HOST).c_str(), serv.getInfo(S_PORT).c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        throw std::exception();
    }
}

void Socket::init() {
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
}

bool Socket::isDupulicate(Socket& other) {
    if (res->ai_addrlen == other.res->ai_addrlen
        && std::memcmp(res->ai_addr, other.res->ai_addr, res->ai_addrlen) == 0)
            return (true);
    return (false);
}

Socket::~Socket() {
    // if (close_on_exit)
    freeaddrinfo(res);
    close(sockid);
}

void Socket::sockBind(addrinfo *res) {
    if (bind(sockid, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        throw std::exception();
    }
}

void Socket::sockListen() {
    if (listen(sockid, MAX_LISTEN) == -1) {
        perror("listen");
        throw std::exception();
    }
}

sock_fd Socket::sockAccept() {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    sock_fd client_fd = accept(sockid, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("accept");
        throw std::exception();
    }
    return (client_fd);
}

sock_fd Socket::getSockid() {
    return sockid;
}


