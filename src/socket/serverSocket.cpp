/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/19 21:37:08 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

ServerSocket::ServerSocket() : Socket() {
}

ServerSocket::ServerSocket(Server& serv) : Socket(serv) {
    servers.push_back(&serv);
}

ServerSocket::ServerSocket(const ServerSocket& other) : Socket(other), servers(other.servers) {
}

ServerSocket::~ServerSocket() {
    for (itrClient it = clients.begin();
            it != clients.end(); it++)
        delete it->second;
}

Server* ServerSocket::getServer() {
    return (servers[0]);
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
    string location = uri.substr(uri.find("/") + 1);
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

map<sock_fd, Client*>& ServerSocket::getClients() {
    return (clients);
}

void ServerSocket::addClient(sock_fd fd) {
    cout    << "new client added on server " 
            << servers[0]->getInfo(S_HOST) << ":"
            << servers[0]->getInfo(S_PORT) << endl;
    clients.insert(std::make_pair(fd, new Client(fd, *this)));
}

void ServerSocket::removeClient(sock_fd fd) {
    cout    << "deleting client on server " 
            << servers[0]->getInfo(S_HOST) << ":"
            << servers[0]->getInfo(S_PORT) << endl;
    itrClient it = clients.find(fd);
    if (it == clients.end())
        return ;
    delete it->second;
    clients.erase(it);
}

ssize_t ServerSocket::sendTo(sock_fd fd) {
    itrClient it = clients.find(fd);
    if (it == clients.end())
        return (-1);
    return (it->second->send());
}

ssize_t ServerSocket::recieveFrom(sock_fd fd) {
    itrClient it = clients.find(fd);
    if (it == clients.end())
        return (-1);
    return (it->second->recieve());
}

bool ServerSocket::isClient(sock_fd fd) {
    if (clients.find(fd) != clients.end())
        return (true);
    return (false);
}

Client* ServerSocket::getClient(sock_fd fd) {
    itrClient it = clients.find(fd);
    if (it == clients.end())
        return (NULL);
    return (it->second);
}
