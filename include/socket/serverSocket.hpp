/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:40:27 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 04:33:59 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include "socket.hpp"

class ServerSocket {
public:
    ServerSocket();
    ServerSocket(Server& serv);
    ServerSocket(const ServerSocket& other);
    ServerSocket& operator=(const ServerSocket& other);
    virtual ~ServerSocket();

    void    init();
    sock_fd sockAccept();
    sock_fd getSockid();
    bool    isDupulicate(ServerSocket& other);
    void    addServer(Server& serv);
    Location&       getLocation(const string& uri);
    deque<Server*>& getServers();
private:
    addrinfo        *res;
    sock_fd         sockid;
    deque<Server*>  servers;
};

#endif // SERVERSOCKET_HPP