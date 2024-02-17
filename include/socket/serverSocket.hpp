/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:40:27 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/12 17:45:44 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include "socket.hpp"

class IServerSocket {
public:
    virtual ~IServerSocket() {};
    virtual void        init() = 0;
    virtual sock_fd     sockAccept() = 0;
    virtual sock_fd     getSockid() = 0;
    virtual bool        isDupulicate(ServerSocket& other) = 0;
    virtual void        addServer(Server& serv) = 0;
    virtual ClientConf& getLocation(const string& uri) = 0;
    virtual deque<Server*>& getServers() = 0;
};

class ServerSocket : public IServerSocket {
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
    ClientConf&       getLocation(const string& uri);
    deque<Server*>& getServers();

    class SOCKET_EXCEPTION: public std::exception {
        string msg;
    public :
        ~SOCKET_EXCEPTION() throw();
        SOCKET_EXCEPTION(const string& error);
        virtual const char* what() const throw();
    };
private:
    addrinfo        *res;
    sock_fd         sockid;
    deque<Server*>  servers;
};

#endif // SERVERSOCKET_HPP