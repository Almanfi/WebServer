/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 04:34:43 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "socket.hpp"

typedef enum {
    NONE,
    READ,
    WRITE,
    ERROR,
    CLOSE
}   cnx_state;

class ServerSocket;

class Client {
public:
    Client(sock_fd fd, ServerSocket& servSock);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();
    
    sock_fd getFd();
    ssize_t send();
    ssize_t recieve();
    void    readBuffer();
    cnx_state&  getState();
    Server&     getServer();
private:
    sock_fd     fd;
    cnx_state   state;
    SBuffer     buffer;
    Request     request;
    string      data;
    ServerSocket& servSock;
};

#endif // CLIENT_HPP