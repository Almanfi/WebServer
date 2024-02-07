// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   client copy.hpp                                    :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: elasce <elasce@student.42.fr>              +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
// /*   Updated: 2024/01/31 05:16:12 by elasce           ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #ifndef CLIENT_HPP
// # define CLIENT_HPP

// # include "socket.hpp"

// class ServerSocket;

// // class Client {
// //     public:
// //     Client(sock_fd fd, ServerSocket& servSock);
// //     Client(const Client& other);
// //     Client& operator=(const Client& other);
// //     ~Client();
    
// //     const cnx_state&    handleState();
// //     ssize_t     send();
// //     ssize_t     recieve();
// //     void        readBuffer();
// //     sock_fd     getFd();
// //     Server&     getServer();
// //     const UUID& getUUID();
// //     void        createFile();
// //     void        openFile();
// // private:
// //     sock_fd     fd;
// //     cnx_state   state;
// //     UUID        uuid;
// //     fstream     file;
// //     SBuffer     buffer;
// //     ServerSocket& servSock;
// //     Request     request;
// // };

// class Response {
// public:
//     Response();
//     void setStatusCode();
//     void setHeader();
//     void setBody();
// private:
//     std::string statusCode;
//     KeyVal header;
//     std::string body;
// };

// class SocketManager {
// public:
//     SocketManager(sock_fd fd);
//     ssize_t send();
//     ssize_t receive();
//     sock_fd getFd();
// private:
//     sock_fd fd;
// };

// class FileManager {
// public:
//     FileManager();
//     void createFile();
//     void openFile();
//     const UUID& getUUID();
// private:
//     fstream file;
//     UUID uuid;
// };

// class RequestManager {
// public:
//     RequestManager();
//     void readBuffer();
// private:
//     SBuffer buffer;
//     Request request;
// };

// class ResponseManager {
// public:
//     ResponseManager();
//     void writeBuffer();
// private:
//     SBuffer buffer;
//     Response response;
// };

// class Client {
// public:
//     Client(SocketManager& socketManager,
//                 FileManager& fileManager,
//                 RequestManager& requestManager,
//                 ResponseManager& responseManager);
    
//     const cnx_state& handleState();
//     // const UUID& getUUID();
// private:
//     cnx_state state;
//     SocketManager& socketManager;
//     FileManager& fileManager;
//     RequestManager& requestManager;
//     ResponseManager& responseManager;
// };

// #endif // CLIENT_HPP