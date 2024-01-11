/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/11 23:36:16 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include "config.hpp"
#include "socket.hpp"
#include <algorithm>



#include <sys/epoll.h>


// void* get_in_addr(struct sockaddr *sa) {
//     if (sa->sa_family == AF_INET) {
//         return &(((struct sockaddr_in*)sa)->sin_addr);
//     }
//     return &(((struct sockaddr_in6*)sa)->sin6_addr);
// }

// in_port_t get_in_port(struct sockaddr *sa) {
//     if (sa->sa_family == AF_INET) {
//         return (((struct sockaddr_in*)sa)->sin_port);
//     }
//     return (((struct sockaddr_in6*)sa)->sin6_port);
// }

// struct client {
//     int fd;
//     std::string ip;
//     int port;
//     string buffer;
//     size_t buffer_size;
//     client(int fd) : fd(fd), ip(""), port(0), buffer(""), buffer_size(0) {}
// };


// int main() {
//     addrinfo hints;
//     addrinfo *res;
//     std::memset(&hints, 0, sizeof(addrinfo));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_flags = AI_PASSIVE;
//     int status = getaddrinfo("127.0.0.1", "9999", &hints, &res);
//     if (status != 0) {
//         std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
//         return 1;
//     }

//     int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//     if (sockfd == -1) {
//         perror("socket");
//         return 1;
//     }
//     int opt = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
//         perror("setsockopt");
//         return 1;
//     }
//     if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
//         perror("bind");
//         return 1;
//     }
//     if (listen(sockfd, 10) == -1) {
//         perror("listen");
//         return 1;
//     }
//     freeaddrinfo(res);
//     int epollfd = epoll_create(10);// test with 0 later
//     if (epollfd == -1) {
//         perror("epoll_create1");
//         return 1;
//     }
//     epoll_event event;
//     event.data.fd = sockfd;
//     event.events = EPOLLIN;
//     if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
//         perror("epoll_ctl");
//         return 1;
//     }
//     std::vector<client> clients;
//     epoll_event events[10];
//     while (true) {
//         cout << "client size: " << clients.size() << endl;
//         int n = epoll_wait(epollfd, events, 10, -1);
//         if (n == -1) {
//             perror("epoll_wait");
//             return 1;
//         }
//         for (int i = 0; i < n; i++) {
//             if (events[i].data.fd == sockfd) {
//                 struct sockaddr_storage client_addr;
//                 socklen_t client_addr_len = sizeof(client_addr);
//                 int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
//                 if (client_fd == -1) {
//                     perror("accept");
//                     return 1;
//                 }
//                 clients.push_back(client(client_fd));
//                 std::cout << "New client connected" << std::endl;
//                 event.data.fd = client_fd;
//                 event.events = EPOLLIN;
//                 if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
//                     perror("epoll_ctl");
//                     return 1;
//                 }
//             } else {
//                 char buffer[1024];
//                 ssize_t bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
//                 if (bytes_read == -1) {
//                     perror("recv");
//                     return 1;
//                 } else if (bytes_read == 0) {
//                     std::cout << "Client disconnected" << std::endl;
//                     close(events[i].data.fd);
//                     epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
//                     for (size_t j = 0; j < clients.size(); j++) {
//                         if (clients[j].fd == events[i].data.fd) {
//                             clients.erase(clients.begin() + j);
//                             break;
//                         }
//                     }
//                 } else {
//                     // Process the received data
//                     std::string request(buffer, bytes_read);
//                     std::cout << "Received request: " << request << std::endl;
//                     // Close the connection after finishing reading the request
//                     close(events[i].data.fd);
//                     epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
//                     for (size_t j = 0; j < clients.size(); j++) {
//                         if (clients[j].fd == events[i].data.fd) {
//                             clients.erase(clients.begin() + j);
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//     }
//     return 0;
// }

int main() {
    // Config conf("config/default.config");
    // conf.read();
    // conf.print();
    Socket socket("127.0.0.1", "9999");
    Epoll epoll(&socket);
    epoll.loop();
    
    return 0;
}
