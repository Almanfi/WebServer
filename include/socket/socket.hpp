/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 22:25:59 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/14 17:33:07 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <deque>
# include <algorithm>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstring>
# include <errno.h>
# include <sys/epoll.h>
# include <sys/stat.h>

# include "config.hpp"

typedef int sock_fd;

#define HOST "host"
#define PORT "port"

#define MAX_LISTEN 10 // max number of clients in the queue

class Socket {
public:
    Socket();
    Socket(const string& ip, const string& port);
    Socket(const Server& serv);
    virtual ~Socket();
    sock_fd sockAccept();
    sock_fd getSockid();
protected:
    void sockBind(addrinfo *res);
    void sockListen();
    sock_fd sockid;
    // bool close_on_exit;
    // Server* server;
};

class SBuffer {
public:
    SBuffer();
    SBuffer(size_t size);
    void bzero();
    size_t size();
    SBuffer(const SBuffer& other);
    SBuffer& operator=(const SBuffer& other);
    ~SBuffer();
    char* operator&();
    char  operator*();
    char* operator+(size_t pos);
    char* operator-(size_t pos);
    char& operator[](size_t pos);
private:
    size_t Size;
    char* buffer;
};

std::ostream& operator<<(std::ostream& os, SBuffer& buffer);

#define RECIEVE_MAX_SIZE 4096

typedef enum {
    NONE,
    READ,
    WRITE,
    ERROR,
    CLOSE
}   cnx_state;

class Client {
public:
    Client(sock_fd fd);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();
    sock_fd getFd();
    ssize_t send();
    ssize_t recieve();
    // void closeOnExit();
private:
    sock_fd     fd;
    // bool        autoClose;
    cnx_state   state;
    SBuffer     buffer;
    // char        buffer[RECIEVE_MAX_SIZE];
    int         buffer_pos;
    char*       old_buffer;
    string      data;
};

class ServerSocket : public Socket {
public:
    ServerSocket();
    ServerSocket(Server& serv);
    ServerSocket(const ServerSocket& other);
    ServerSocket& operator=(const ServerSocket& other);
    virtual ~ServerSocket();
    Server* getServer();
    map<sock_fd, Client*>& getClients();
    void addClient(sock_fd fd);
    void removeClient(sock_fd fd);
    bool isClient(sock_fd fd);
    ssize_t sendTo(sock_fd fd);
    ssize_t recieveFrom(sock_fd fd);
private:
    typedef  map<sock_fd, Client*>::iterator itrClient;
    Server* server;
    map<sock_fd, Client*> clients;
};

class Epoll {
public:
    Epoll(Config& config);
    // Epoll(Socket* socket);
    ~Epoll();
    void loop();
    void checkEvents(int n);
    void removeSocket(Socket* socket);
    void wait();
    void setServer(Server* server);
private:
    typedef map<sock_fd, ServerSocket*>::iterator itrServSock;
    void addEvent(sock_fd fd, uint32_t events);
    void delEvent(sock_fd fd);
    bool eventOnServer(sock_fd fd);
    map<sock_fd, ServerSocket*> servSockets;
    ServerSocket*   servSock;
    epoll_event events[10];
    epoll_event event;
    // Socket* socket;
    int epollfd;
    // vector<int> fds;
    // Server* server;
};

#endif // SOCKET_HPP