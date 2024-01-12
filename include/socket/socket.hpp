/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 22:25:59 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/12 16:29:16 by maboulkh         ###   ########.fr       */
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


class Socket {
public:
    Socket(const string& ip, const string& port);
    Socket(Server& serv);
    // Socket(int sockid);
    ~Socket();
    int sockAccept();
    void sockClose();
    int getSockid();
    void setServer(Server* server);
    void setEpollfd(int epollfd);
private:
    void sockBind(addrinfo *res);
    void sockListen();
    int sockid;
    Server* server;
};

class Epoll {
public:
    Epoll(Config& config);
    Epoll(Socket* socket);
    ~Epoll();
    void loop();
    void addSocket(Socket* socket);
    void removeSocket(Socket* socket);
    void wait();
    void setServer(Server* server);
private:
    // std::vector<client> clients;
    deque<Socket*> servSocket;
    epoll_event events[10];
    epoll_event event;
    Socket* socket;
    int epollfd;
    vector<int> fds;
    // struct epoll_event event;
    // struct epoll_event* events;
    Server* server;
};

#endif // SOCKET_HPP