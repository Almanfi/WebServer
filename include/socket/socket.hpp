/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 22:25:59 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/13 13:48:07 by maboulkh         ###   ########.fr       */
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

#define MAX_LISTEN 10 // max number of clients in the queue


class Socket {
public:
    Socket(const string& ip, const string& port);
    Socket(Server& serv);
    ~Socket();
    sock_fd sockAccept();
    sock_fd getSockid();
private:
    void sockBind(addrinfo *res);
    void sockListen();
    sock_fd sockid;
    // Server* server;
};

class Epoll {
public:
    Epoll(Config& config);
    Epoll(Socket* socket);
    ~Epoll();
    void loop();
    void checkEvents(int n);
    void removeSocket(Socket* socket);
    void wait();
    void setServer(Server* server);
private:
    void addEvent(sock_fd fd, uint32_t events);
    void delEvent(sock_fd fd);
    bool eventOnServer(sock_fd fd);
    deque<Socket*> servSocket;
    epoll_event events[10];
    epoll_event event;
    Socket* socket;
    int epollfd;
    vector<int> fds;
    Server* server;
};

#endif // SOCKET_HPP