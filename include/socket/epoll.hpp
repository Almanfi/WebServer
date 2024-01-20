/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:43:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 04:30:23 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include "socket.hpp"

class Epoll {
public:
    Epoll(Config& config);
    ~Epoll();
    void loop();
    void checkEvents(int n);
private:
    void addClient(sock_fd fd, uint32_t events);
    void delClient(sock_fd fd);
    void addEvent(sock_fd fd, uint32_t events);
    void delEvent(sock_fd fd);
    bool eventOnServer(sock_fd fd);
    void handleClient(int i);
    typedef map<sock_fd, ServerSocket*>::iterator itrServSock;
    map<sock_fd, ServerSocket*> servSockets;
    typedef  map<sock_fd, Client*>::iterator itrClient;
    map<sock_fd, Client*> clients;
    ServerSocket*   servSock;
    Client*         client;
    epoll_event     events[10];
    epoll_event     event;
    int             epollfd;
};

#endif // EPOLL_HPP