/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdiraa <fdiraa@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/04 17:25:41 by fdiraa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "socket.hpp"

class ServerSocket;
class Client {
    public:
    Client(sock_fd fd, ServerSocket& servSock);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();
    
    const cnx_state&    handleState();
    ssize_t     send();
    ssize_t     recieve();
    void        readBuffer();
    sock_fd     getFd();
    Server&     getServer();
    const UUID& getUUID();
    void        createFile();
    void        openFile();
    friend class Response;
private:
    sock_fd     fd;
    cnx_state   state;
    UUID        uuid;
    fstream     file;
    SBuffer     buffer;
    ServerSocket& servSock;
    Request     request;
    Response    response;
    string      data;
};

#endif // CLIENT_HPP