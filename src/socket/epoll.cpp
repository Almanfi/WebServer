/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:28 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/17 18:44:04 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Epoll::Epoll() {
}

Epoll::~Epoll() {
    close(epollfd);
    for (itrServSock it = servSockets.begin();
            it != servSockets.end(); it++)
        delete it->second;
}

void Epoll::init(Config& config) {
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
        try {
            tmp->init();
        }
        catch (const ServerSocket::SOCKET_EXCEPTION& e) {
            delete tmp;
            throw std::runtime_error(
                string("could not init server " + servers[i].getInfo(S_HOST)
                        + ":" + servers[i].getInfo(S_PORT)).c_str());
        }
        servSockets.insert(std::make_pair(tmp->getSockid(), tmp));
        addEvent(tmp->getSockid(), EPOLLIN); // TODO no need to add EPOLLOUT right?
    }
    cout << "number of servers listening: " << servSockets.size() << endl;
    for (itrServSock it = servSockets.begin(); it != servSockets.end(); it++) {
        cout << "server " << it->second->getServers()[0]->getInfo(S_HOST) << ":"
            << it->second->getServers()[0]->getInfo(S_PORT) << endl;
    }
    loop();
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
    IClientResourceManagerFactory* clientRMFactory = new ClientResourceManagerFactory();
    IClientResourceManagerFacade* clientRMF = clientRMFactory->createFacade(fd, *servSock);
    Client* client = new Client(clientRMF);
    clients.insert(std::make_pair(fd, client));
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
    // if (events[i].events & EPOLLIN)
    //     client->setState(READ);
    // else
    //     client->setState(WRITE);
    // events[i].events & EPOLLIN;
    if (client->handleState() == CLOSE) {
        cout << "++++++++++++ closing ++++++++++++" << endl;
        delClient(events[i].data.fd);
    }
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
