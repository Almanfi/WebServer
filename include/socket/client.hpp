/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/23 00:07:09 by maboulkh         ###   ########.fr       */
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
    void        createFile();
    void        openFile();
private:
    sock_fd     fd;
    cnx_state   state;
    SBuffer     buffer;
    Request     request;
    string      data;
    size_t      contentLength;
    ServerSocket& servSock;
    UUID        uuid;
    fstream     file;
};

#endif // CLIENT_HPP