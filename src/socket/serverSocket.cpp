/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/23 15:38:04 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

ServerSocket::ServerSocket() {
}

ServerSocket::ServerSocket(Server& serv) {
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
    servers.push_back(&serv);
}

ServerSocket::ServerSocket(const ServerSocket& other) : res(NULL), sockid(other.sockid), servers(other.servers) {
    // this whole mess is just to avoid ashallow copy for res
    addrinfo hints;
    std::memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(servers[0]->getInfo(S_HOST).c_str(), servers[0]->getInfo(S_PORT).c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        throw std::exception();
    }
}

ServerSocket::~ServerSocket() {
    // for (itrClient it = clients.begin();
    //         it != clients.end(); it++)
    //     delete it->second;
    freeaddrinfo(res);
    close(sockid);
}

void ServerSocket::init() {
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
    if (bind(sockid, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        throw std::exception();
    }
    if (listen(sockid, MAX_LISTEN) == -1) {
        perror("listen");
        throw std::exception();
    }
}

bool ServerSocket::isDupulicate(ServerSocket& other) {
    if (res->ai_addrlen == other.res->ai_addrlen
        && std::memcmp(res->ai_addr, other.res->ai_addr, res->ai_addrlen) == 0)
            return (true);
    return (false);
}

sock_fd ServerSocket::sockAccept() {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    sock_fd client_fd = accept(sockid, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("accept");
        throw std::exception();
    }
    return (client_fd);
}

sock_fd ServerSocket::getSockid() {
    return sockid;
}

void ServerSocket::addServer(Server& serv) {
    for (size_t i = 0; i < serv.server_name.size(); i++) {
        for (size_t j = 0; j < servers.size(); j++) {
            if (std::find(servers[j]->server_name.begin(), servers[j]->server_name.end(),
                serv.server_name[i]) != servers[j]->server_name.end())
                throw std::runtime_error("conflicting server names");
        }
    }
    servers.push_back(&serv);
}

Location& ServerSocket::getLocation(const string& uri) {
    string server_name = uri.substr(0, uri.find("/"));
    string location = uri.substr(uri.find("/"));
    Server* serv = NULL;
    for (deque<Server*>::iterator it = servers.begin(); it != servers.end(); it++) {
        for (size_t i = 0; i < (*it)->server_name.size(); i++) {
            if ((*it)->server_name[i] == server_name) {
                serv = *it;
                break;
            }
        }
        if (server_name == (*it)->listenIp) {
            serv = *it;
            break;
        }
    }
    if (!serv) {
        throw std::runtime_error("Error: server not found");
    }
    return (serv->locations.find("/")->second->getLocation(location));
}

deque<Server*>& ServerSocket::getServers() {
    return (servers);
}
