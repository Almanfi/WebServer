/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:28 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/19 20:53:13 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Epoll::Epoll(Config& config) {
    std::memset(&event, 0, sizeof(event));
    deque<Server>& servers =  config.getServers();
    epollfd = epoll_create(10);// ignored nowdays, it just need to be greater than 0
    if (epollfd < 0) {
        perror("epoll_create");
        throw std::exception();
    }
    for (size_t i = 0; i < servers.size(); i++) {
        ServerSocket*   tmp = new ServerSocket(servers[i]);
        for (itrServSock it = servSockets.begin(); it != servSockets.end(); it++) {
            if (tmp->isDupulicate(*it->second)) {
                it->second->addServer(servers[i]);
                delete tmp;
                tmp = NULL;
                cout << "duplicate server found" << endl;
                cout << "server " << servers[i].getInfo(S_HOST) << ":"
                    << servers[i].getInfo(S_PORT) << " added to server "
                    << it->second->getServer()->getInfo(S_HOST) << ":"
                    << it->second->getServer()->getInfo(S_PORT) << endl;
                break ;
            }
        }
        if (!tmp)
            continue ;
        tmp->init();
        servSockets.insert(std::make_pair(tmp->getSockid(), tmp));
        addEvent(tmp->getSockid(), EPOLLIN);
    }
    cout << "number of servers listening: " << servSockets.size() << endl;
}

// Epoll::Epoll(Socket* socket) : socket(socket) {
//     std::memset(&event, 0, sizeof(event));
//     epollfd = epoll_create(10); // ignored nowdays, it just need to be greater than 0
//     if (epollfd < 0) {
//         perror("epoll_create");
//         throw std::exception();
//     }
//     addEvent(socket->getSockid(), EPOLLIN);
//     // event.data.fd = socket->getSockid();
//     // event.events = EPOLLIN;
//     // if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket->getSockid(), &event) == -1) {
//     //     perror("epoll_ctl");
//     //     throw std::exception();
//     // }
// }

Epoll::~Epoll() {
    close(epollfd);
    for (itrServSock it = servSockets.begin();
            it != servSockets.end(); it++)
        delete it->second;
}

void Epoll::addEvent(sock_fd fd, uint32_t events) {
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl of adding client");
        throw std::exception();
    }
}

void Epoll::delEvent(sock_fd fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("epoll_ctl");
        throw std::exception();
    }
}

bool Epoll::eventOnServer(sock_fd fd) {
    client = NULL;
    itrServSock it = servSockets.find(fd);
    if (it != servSockets.end())
    {
        servSock = it->second;
        return (true);
    }
    for (itrServSock it = servSockets.begin();
            it != servSockets.end(); it++)
    {
        client = it->second->getClient(fd);
        if (client)
            return (false);
    }
    throw std::runtime_error("eventOnServer: fd not found");
}

void Epoll::handleClient(int i) {
    cnx_state& state = client->getState();
    if (state == NONE)
        state = READ;
    cnx_state prevState = state;
    switch (state) {
        case READ:
            if (client->recieve() == 0)
                state = CLOSE;
            break ;
        case WRITE:
            client->send();
            break ;
        case CLOSE:
            cout << "++++++++++++ closing ++++++++++++" << endl;
            delEvent(events[i].data.fd);
            servSock->removeClient(events[i].data.fd);
            break ;
        default:
            throw std::runtime_error("Client::handle: invalid state");
    }
    if (prevState == READ && state == WRITE) {
        int fd = events[i].data.fd;
        event.data.fd = fd;
        event.events = EPOLLOUT;
        if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event)) {
            perror("epoll_ctl to change event to EPOLLOUT");
            throw std::exception();
        }
    }
    // if state chaged from READ to WRITE  change event to EPOLLOUT
}

void Epoll::checkEvents(int n) {
    for (int i = 0; i < n; i++) {
        if (eventOnServer(events[i].data.fd)) {
            sock_fd client_fd = servSock->sockAccept();
            servSock->addClient(client_fd);
            addEvent(client_fd, EPOLLIN);
            std::cout << "New client connected" << std::endl;
        } else
            handleClient(i);
    }
}

void Epoll::loop() {
    while (true) {
        int n = epoll_wait(epollfd, events, 10, -1);
        if (n == -1) {
            perror("epoll_wait");
            throw std::exception();
        }
        checkEvents(n);
    }
}
