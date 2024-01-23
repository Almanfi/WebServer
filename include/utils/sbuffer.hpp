/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbuffer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:18:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/24 00:34:32 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SBUFFER_HPP
# define SBUFFER_HPP

# include "socket.hpp"

#define SBUFFER_SIZE 65536

class SBuffer {
public:
    SBuffer();
    SBuffer(const SBuffer& other);
    SBuffer& operator=(const SBuffer& other);
    ~SBuffer();
    void bzero();
    ssize_t recv(sock_fd fd, int flags);
    ssize_t begin();
    ssize_t end();
    void moveDataToStart();
    size_t find(const string& str, size_t pos);

    ssize_t freeSpace();
    ssize_t capacity();
    ssize_t size();
    void clear();
    bool empty();
    bool skip(ssize_t offset);
    char* operator&();
    char&  operator*();
    char* operator+(size_t i);
    char* operator-(size_t i);
    char& operator[](size_t i);
private:
    ssize_t start;
    ssize_t count;
    char buffer[SBUFFER_SIZE]; // TODO for optimization mallc size + 2 to avoid memmove when buffer is full
};

std::ostream& operator<<(std::ostream& os, SBuffer& buffer);

#endif // SBUFFER_HPP

