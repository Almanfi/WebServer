/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/15 22:23:33 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

ServerSocket::ServerSocket() : Socket() {
}

ServerSocket::ServerSocket(Server& serv) : Socket(serv), server(&serv) {
}

ServerSocket::ServerSocket(const ServerSocket& other) : Socket(other), server(other.server) {
}

ServerSocket::~ServerSocket() {
    for (itrClient it = clients.begin();
            it != clients.end(); it++)
        delete it->second;
}

Server* ServerSocket::getServer() {
    return (server);
}

map<sock_fd, Client*>& ServerSocket::getClients() {
    return (clients);
}

void ServerSocket::addClient(sock_fd fd) {
    cout    << "new client added on server " 
            << server->getInfo(S_HOST) << ":"
            << server->getInfo(S_PORT) << endl;
    clients.insert(std::make_pair(fd, new Client(fd)));
}

void ServerSocket::removeClient(sock_fd fd) {
    cout    << "deleting client on server " 
            << server->getInfo(S_HOST) << ":"
            << server->getInfo(S_PORT) << endl;
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
