/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbuffer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:18:58 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/28 02:04:21 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SBUFFER_HPP
# define SBUFFER_HPP

# include "socket.hpp"

#define SBUFFER_SIZE 65536

class ISBuffer {
public:
    virtual ~ISBuffer() {};
    virtual void bzero() = 0;
    virtual ssize_t recv(sock_fd fd, int flags) = 0;
    virtual ssize_t send(sock_fd fd, int flags) = 0;
    virtual ssize_t begin() = 0;
    virtual ssize_t end() = 0;
    virtual void moveDataToStart() = 0;
    virtual size_t find(const string& str, size_t pos) = 0;
    virtual ssize_t freeSpace() = 0;
    virtual ssize_t capacity() = 0;
    virtual ssize_t size() = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    virtual bool skip(ssize_t offset) = 0;
    virtual char* operator&() = 0;
    virtual char&  operator*() = 0;
    virtual size_t write(const string& str) = 0;
};

class SBuffer  : public ISBuffer {
public:
    SBuffer();
    SBuffer(const SBuffer& other);
    SBuffer& operator=(const SBuffer& other);
    ~SBuffer();
    void bzero();
    ssize_t recv(sock_fd fd, int flags);
    ssize_t send(sock_fd fd, int flags);
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
    size_t write(const string& str);
private:
    ssize_t start;
    ssize_t count;
    char buffer[SBUFFER_SIZE];
};

std::ostream& operator<<(std::ostream& os, SBuffer& buffer);

#endif // SBUFFER_HPP

