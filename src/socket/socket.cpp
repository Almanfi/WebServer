/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 12:36:29 by bamrouch          #+#    #+#             */
/*   Updated: 2024/01/14 17:34:36 by maboulkh         ###   ########.fr       */
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
    addrinfo *res;
    addrinfo hints;
    std::memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // If node is not NULL, then the AI_PASSIVE flag is ignored.
    int status = getaddrinfo(serv.getInfo(HOST).c_str(), serv.getInfo(PORT).c_str(), &hints, &res);
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
    // if (close_on_exit)
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

// Epoll::Epoll(Config& config) {
//     std::memset(&event, 0, sizeof(event));
//     deque<Server>& servers =  config.getServers();
//     epollfd = epoll_create(10);// ignored nowdays, it just need to be greater than 0
//     if (epollfd < 0) {
//         perror("epoll_create");
//         throw std::exception();
//     }
//     for (size_t i = 0; i < servers.size(); i++) {
//         servSocket.push_back(new Socket(servers[i]));
//         addEvent(servSocket.back()->getSockid(), EPOLLIN);
//         // event.data.fd = servSocket.back()->getSockid();
//         // event.events = EPOLLIN;
//         // if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servSocket.back()->getSockid(), &event) == -1) {
//         //     perror("epoll_ctl");
//         //     throw std::exception();
//         // }
//     }
//     cout << "number of servers listening: " << servSocket.size() << endl;
// }

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
        
        // std::make_pair(tmp.getSockid(), ServerSocket(tmp));
        servSockets.insert(std::make_pair(tmp->getSockid(), tmp));
        // servSockets[tmp.getSockid()].closeOnExit();
        addEvent(tmp->getSockid(), EPOLLIN);
        // servSocket.push_back(new Socket(servers[i]));
        // addEvent(servSocket.back()->getSockid(), EPOLLIN);
        // event.data.fd = servSocket.back()->getSockid();
        // event.events = EPOLLIN;
        // if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servSocket.back()->getSockid(), &event) == -1) {
        //     perror("epoll_ctl");
        //     throw std::exception();
        // }
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
    // for (size_t i = 0; i < servSocket.size(); i++) {
    //     delete &servSocket[i];
    // }
}

void Epoll::addEvent(sock_fd fd, uint32_t events) {
    // fds.push_back(fd);
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
    // for (size_t j = 0; j < fds.size(); j++) {
    //     if (fds[j] == fd) {
    //         fds.erase(fds.begin() + j);
    //         break;
    //     }
    // }
}

// bool Epoll::eventOnServer(sock_fd fd) {
//     size_t index;
//     for (index = 0; index < servSocket.size(); index++) {
//         if (fd == servSocket[index]->getSockid()) {
//             socket = servSocket[index];
//             break;
//         }
//     }
//     if (index < servSocket.size())
//         return true;
//     return false;
// }

bool Epoll::eventOnServer(sock_fd fd) {
    itrServSock it = servSockets.find(fd);
    if (it == servSockets.end())
    {
        for (itrServSock it = servSockets.begin();
                it != servSockets.end(); it++)
        {
            if (it->second->isClient(fd))
            {
                servSock = it->second;
                return (false);
            }
        }
        throw std::runtime_error("eventOnServer: fd not found");
    }
    servSock = it->second;
    return (true);
}

void Epoll::checkEvents(int n) {
    for (int i = 0; i < n; i++) {
        if (eventOnServer(events[i].data.fd)) {
            sock_fd client_fd = servSock->sockAccept();
            servSock->addClient(client_fd);
            addEvent(client_fd, EPOLLIN);
            std::cout << "New client connected" << std::endl;
        } else {
            char buffer[1024];
            ssize_t bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
            if (bytes_read == -1) {
                perror("recv");
                throw std::exception();
            } else if (bytes_read == 0) {
                std::cout << "Client disconnected" << std::endl;
                delEvent(events[i].data.fd);
                servSock->removeClient(events[i].data.fd);
                // close(events[i].data.fd);
            } else {
                std::string request(buffer, bytes_read);
                std::cout << "Received request: " << request << std::endl;
            }
        }
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


SBuffer::SBuffer() : Size(RECIEVE_MAX_SIZE), buffer(new char[RECIEVE_MAX_SIZE]) {
}

SBuffer::SBuffer(size_t size) : Size(size) {
    buffer = new char[size];
}

SBuffer::SBuffer(const SBuffer& other) {
    Size = other.Size;
    buffer = new char[Size];
    std::memcpy(buffer, other.buffer, Size);
}

SBuffer& SBuffer::operator=(const SBuffer& other) {
    if (this != &other) {
        delete[] buffer;
        Size = other.Size;
        buffer = new char[Size];
        std::memcpy(buffer, other.buffer, Size);
    }
    return (*this);
}

SBuffer::~SBuffer() {
    delete[] buffer;
}

char* SBuffer::operator&() {
    return (buffer);
}

char SBuffer::operator*() {
    return (*buffer);
}

char* SBuffer::operator+(size_t pos) {
    return (buffer + pos);
}

char* SBuffer::operator-(size_t pos) {
    return (buffer - pos);
}

char& SBuffer::operator[](size_t pos) {
    return (buffer[pos]);
}

void SBuffer::bzero() {
    std::memset(buffer, 0, Size);
}

size_t SBuffer::size() {
    return (Size);
}

std::ostream& operator<<(std::ostream& os, SBuffer& buffer) {
    os.write(&buffer, buffer.size());
    return (os);
}

Client::Client(sock_fd fd) : fd(fd), state(NONE), buffer_pos(0), old_buffer(NULL) {
}

Client::Client(const Client& other) {
    fd = other.fd;
    state = other.state;
    buffer = other.buffer;
    buffer_pos = other.buffer_pos;
    old_buffer = other.old_buffer;
    data = other.data;
}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        fd = other.fd;
        state = other.state;
        buffer = other.buffer;
        buffer_pos = other.buffer_pos;
        old_buffer = other.old_buffer;
        data = other.data;
    }
    return (*this);
}

Client::~Client() {
    close(fd);
}

sock_fd Client::getFd() {
    return (fd);
}

ssize_t Client::send() {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    ssize_t bytes_sent = ::send(fd, response.c_str(), response.size(), 0);
    if (bytes_sent == -1) {
        perror("send");
        throw std::exception();
    }
    return (bytes_sent);
}

ssize_t Client::recieve() {
    ssize_t bytes_read = recv(fd, &buffer, buffer.size(), 0);
    if (bytes_read == -1) {
        perror("recv");
        throw std::exception();
    } else if (bytes_read == 0) {
        std::cout << "Client disconnected" << std::endl;
        // delEvent(events[i].data.fd);
    } else {
        cout << "Received request: " << buffer << std::endl;
    }
    return (bytes_read);
}

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
            << server->getInfo(HOST) << ":"
            << server->getInfo(PORT) << endl;
    clients.insert(std::make_pair(fd, new Client(fd)));
}

void ServerSocket::removeClient(sock_fd fd) {
    cout    << "deleting client on server " 
            << server->getInfo(HOST) << ":"
            << server->getInfo(PORT) << endl;
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

