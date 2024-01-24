/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:28 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/24 20:46:34 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Epoll::Epoll(Config& config) {
    if (MAX_EVENTS < 1) // TODO check on macros at compile time
        throw std::runtime_error("Epoll::Epoll: MAX_EVENTS must be greater than 0");
    Header::initHeadersRules();
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
                    << it->second->getServers()[0]->getInfo(S_HOST) << ":"
                    << it->second->getServers()[0]->getInfo(S_PORT) << endl;
                break;
            }
        }
        if (!tmp)
            continue ;
        tmp->init();
        servSockets.insert(std::make_pair(tmp->getSockid(), tmp));
        addEvent(tmp->getSockid(), EPOLLIN); // TODO no need to add EPOLLOUT right?
    }
    cout << "number of servers listening: " << servSockets.size() << endl;
}

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

void Epoll::addClient(sock_fd fd, uint32_t events) {
    cout    << "new client added on server " 
            << servSock->getServers()[0]->getInfo(S_HOST) << ":"
            << servSock->getServers()[0]->getInfo(S_PORT) << endl;
    clients.insert(std::make_pair(fd, new Client(fd, *servSock)));
    cout << "client UUID : " << clients[fd]->getUUID().getStr() << endl;
    addEvent(fd, events);
}

void Epoll::delClient(sock_fd fd) {
    cout    << "deleting client on server "
            << client->getServer().getInfo(S_HOST) << ":"
            << client->getServer().getInfo(S_PORT) << endl;
    cout    << "client UUID : " << client->getUUID().getStr() << endl;
    itrClient it = clients.find(fd);
    if (it == clients.end())
        return ;
    delEvent(fd);
    delete it->second;
    clients.erase(it);
}

bool Epoll::eventOnServer(sock_fd fd) {
    itrServSock servIt = servSockets.find(fd);
    if (servIt != servSockets.end())
    {
        servSock = servIt->second;
        return (true);
    }
    itrClient clientIt = clients.find(fd);
    if (clientIt != clients.end())
    {
        client = clientIt->second;
        return (false);
    }
    throw std::runtime_error("Epoll::eventOnServer: invalid fd");
}

void Epoll::handleClient(int i) {
    cnx_state& state = client->getState();
    if (state == NONE)
        state = READ;
    // cnx_state prevState = state;
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
            delClient(events[i].data.fd);
            break ;
        default:
            throw std::runtime_error("Client::handle: invalid state");
    }
    // if (prevState == READ && state == WRITE) {
    //     int fd = events[i].data.fd;
    //     event.data.fd = fd;
    //     event.events = EPOLLOUT;
    //     if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event)) {
    //         perror("epoll_ctl to change event to EPOLLOUT");
    //         throw std::exception();
    //     }
    // }
}

void Epoll::checkEvents(int n) {
    for (int i = 0; i < n; i++) {
        if (eventOnServer(events[i].data.fd)) {
            // if (events[i].events & EPOLLIN == false)
            //     continue ;// TODO serverr asdded as EPOLLIN only now
            if (clients.size() >= MAX_EVENTS) {
                std::cout << "Max number of clients reached" << std::endl;
                continue ;
            }
            sock_fd client_fd = servSock->sockAccept();
            addClient(client_fd, EPOLLIN | EPOLLOUT);
            std::cout << "New client connected" << std::endl;
        } else
            handleClient(i);
    }
}

void Epoll::loop() {
    while (true) {
        int n = epoll_wait(epollfd, events, MAX_EVENTS, -1);// TODO change timeout
        if (n == -1) {
            perror("epoll_wait");
            throw std::exception();
        }
        checkEvents(n);
    }
}
