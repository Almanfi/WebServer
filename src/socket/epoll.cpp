/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:40:28 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 16:02:47 by maboulkh         ###   ########.fr       */
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
    epollfd = epoll_create(10);
    if (epollfd < 0) {
        throw std::runtime_error("could not create epoll");
    }
    for (size_t i = 0; i < servers.size(); i++) {
        ServerSocket*   tmp = new ServerSocket(servers[i]);
        for (itrServSock it = servSockets.begin(); it != servSockets.end(); it++) {
            if (tmp->isDupulicate(*it->second)) {
                try {
                    it->second->addServer(servers[i]);
                } catch (const std::exception& e) {
                    delete tmp;
                    throw std::runtime_error(
                        string("could not add server " + servers[i].getInfo(S_HOST)
                                + ":" + servers[i].getInfo(S_PORT)).c_str());
                }
                delete tmp;
                tmp = NULL;
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
                string(e.what() + servers[i].getInfo(S_HOST)
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
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
        throw std::runtime_error("Epoll::addEvent: epoll_ctl failed");
}

void Epoll::delEvent(sock_fd fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Epoll::delEvent: epoll_ctl failed");
}

void Epoll::addClient(sock_fd fd, uint32_t events) {
   cout    << endl << "+ new client added on server " 
            << servSock->getServers()[0]->getInfo(S_HOST) << ":"
            << servSock->getServers()[0]->getInfo(S_PORT) << endl;
    IClientResourceManagerFactory* clientRMFactory = new ClientResourceManagerFactory();
    IClientResourceManagerFacade* clientRMF = clientRMFactory->createFacade(fd, *servSock);
    Client* client = new Client(clientRMF);
    clients.insert(std::make_pair(fd, client));
    cout << "+ client UUID : " << clients[fd]->getUUID().getStr() << endl;
    addEvent(fd, events);
}

void Epoll::delClient(sock_fd fd) {
    itrClient it = clients.find(fd);
    if (it == clients.end()) {
        close(fd);
        return ;
    }
    cout    << endl <<  "- deleting client on server "
                << client->getServer().getInfo(S_HOST) << ":"
                << client->getServer().getInfo(S_PORT) << endl;
    cout    << "- client UUID : " << client->getUUID().getStr() << endl;
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
    try {
        bool isEpollIn = events[i].events & EPOLLIN;
        if (client->handleState(isEpollIn) == CLOSE) {
            delClient(events[i].data.fd);
        }
    }
    catch (const std::exception& e) {
        delClient(events[i].data.fd);
        std::cerr << "client was deleted: " << e.what() << std::endl;
    }
}

void Epoll::checkEvents(int n) {
    for (int i = 0; i < n; i++) {
        if (eventOnServer(events[i].data.fd)) {
            if (clients.size() >= MAX_EVENTS) {
                std::cout << "Max number of clients reached" << std::endl;
                continue ;
            }
            sock_fd client_fd;
            try {
                client_fd = servSock->sockAccept();
                addClient(client_fd, EPOLLIN | EPOLLOUT);
            }
            catch (const std::exception& e) {
                std::cerr << "- client couldn't be added: " << e.what() << std::endl;
                if (client_fd != -1) {
                    delClient(client_fd);
                }
            }
        } else
            handleClient(i);
    }
}

void Epoll::handleTimeOut() {
    vector<sock_fd> client_with_timeout;
    for (map<sock_fd, Client*>::iterator it = clients.begin();
                                    it != clients.end(); it++) {
        if (it->second->checkTimeout()) {
            client_with_timeout.push_back(it->first);
        }
    }
    for (size_t i = 0; i < client_with_timeout.size(); i++) {
        delClient(client_with_timeout[i]);
    }
}

void Epoll::loop() {
    int timeout = 3000;
    while (true) {
        int n = epoll_wait(epollfd, events, MAX_EVENTS, timeout);// TODO change timeout
        if (n == -1)
            throw std::runtime_error("epoll_wait failed");
        checkEvents(n);
        handleTimeOut();
    }
}
