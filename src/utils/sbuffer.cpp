/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbuffer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:22:27 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/28 01:52:48 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "sbuffer.hpp"


#include "socket.hpp"

SBuffer::SBuffer() : start(0), count(0) {
}

SBuffer::SBuffer(const SBuffer& other) : start(other.start), count(other.count) {
    std::memcpy(buffer + start, other.buffer + start, count);
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

size_t SBuffer::write(const string& str) {
    if (static_cast<size_t>(freeSpace()) < str.size())
        moveDataToStart();
    size_t size = std::min(str.size(), static_cast<size_t>(freeSpace()));
    std::memcpy(buffer + end(), str.c_str(), size);
    count += size;
    return (size);
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

ssize_t SBuffer::capacity() {
    return (SBUFFER_SIZE);
}

ssize_t SBuffer::freeSpace() {
    return (SBUFFER_SIZE - end());
}

bool SBuffer::empty() {
    return (count == 0);
}

void SBuffer::bzero() {
    std::memset(buffer, 0, SBUFFER_SIZE);
}
void SBuffer::moveDataToStart() {
    std::memmove(buffer, buffer + start, count);
    start = 0;
}

size_t SBuffer::find(const string& str, size_t pos) {
    for (ssize_t i = pos; i < count; i++) {
        if (std::memcmp(buffer + start + i, str.c_str(), str.size()) == 0)
            return (i);
    }
    return (string::npos);
}

ssize_t SBuffer::recv(sock_fd fd, int flags) {
    ssize_t recvSize = ::recv(fd, buffer + end(), freeSpace(), flags);
    if (recvSize == -1)
        throw std::runtime_error("could not receive data");
    count += recvSize;
    return (recvSize);
}

ssize_t SBuffer::send(sock_fd fd, int flags) {
    ssize_t sendSize = ::send(fd, buffer + start, count, flags);
    if (sendSize == -1) {
        throw std::runtime_error("could not send data");
    }
    start += sendSize;
    count -= sendSize;
    return (sendSize);
}

bool SBuffer::skip(ssize_t offset) {
    if (offset > count)
        return (false);
    else if (offset == count) {
        clear();
        return (true);
    }
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
    // os << "|" << buffer.size() << "|" << endl;// for debuging
    return (os);
}
