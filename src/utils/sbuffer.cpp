/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbuffer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:22:27 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/19 23:35:35 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "sbuffer.hpp"


#include "socket.hpp"


SBuffer::SBuffer() : start(0), count(0) {
}

SBuffer::SBuffer(const SBuffer& other) : start(other.start), count(other.count) {
    std::memcpy(buffer + start, other.buffer + start, count); // TODO is this safe? debug
}

SBuffer& SBuffer::operator=(const SBuffer& other) {
    if (this != &other) {
        count = other.count;
        start = other.start;
        std::memcpy(buffer + start, other.buffer + start, count);
    }
    return (*this);
}

SBuffer::~SBuffer() {
}

char* SBuffer::operator&() {
    return (buffer + start);
}

char& SBuffer::operator*() {
    return (*(buffer + start));
}

char* SBuffer::operator+(size_t i) {
    return (buffer + start + i);
}

char* SBuffer::operator-(size_t i) {
    return (buffer + start - i);
}

char& SBuffer::operator[](size_t i) {
    return (buffer[start + i]);
}

ssize_t SBuffer::begin() {
    return (start);
}

ssize_t SBuffer::size() {
    return (count);
}

ssize_t SBuffer::end() {
    return (start + count);
}

bool SBuffer::empty() {
    return (count == 0);
}

void SBuffer::bzero() {
    std::memset(buffer, 0, SBUFFER_SIZE);
}

ssize_t SBuffer::recv(sock_fd fd, int flags) {
    // if (end() == SBUFFER_SIZE) // TODO is this necesary? might delete data
    //     clear(); //
    ssize_t recvSize = ::recv(fd, buffer + end(), SBUFFER_SIZE - end(), flags);
    if (recvSize == -1) {
        perror("recv1");
        throw std::exception();
    }
    count += recvSize;
    return (recvSize);
}

bool SBuffer::skip(ssize_t offset) {
    if (offset > count)
        return (false);
    count -= offset;
    start += offset;
    return (true);
}

void SBuffer::clear() {
    start = 0;
    count = 0;
}

std::ostream& operator<<(std::ostream& os, SBuffer& buffer) {
    os.write(&buffer, buffer.size());
    os << "|" << buffer.size() << "|" << endl;// for debuging
    return (os);
}
