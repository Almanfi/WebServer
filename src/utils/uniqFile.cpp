/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniqFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 00:02:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/28 01:53:27 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uniqFile.hpp"

UniqFile::UniqFile(string rootPath, Iuuid &uuid) : _path(rootPath), _uuid(uuid) {
    while (access((rootPath + "/" + _uuid.getStr()).c_str(), F_OK) == 0) {
        _uuid.regen();
    }
    _path = rootPath + "/" + _uuid.getStr();
    createFile();
}

UniqFile::~UniqFile() {
    close();
    remove();
}

void UniqFile::createFile() {
    _file.open(_path.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
    if (!_file.is_open()) {
        throw std::runtime_error("Error: unique file could not be created");
    }
    _file.close();
}

void UniqFile::open() {
    _file.open(_path.c_str(), std::ios::in | std::ios::out | std::ios::app);
}

void UniqFile::close() {
    _file.close();
}

void UniqFile::remove() {
    _file.close();
    std::remove(_path.c_str());
}

string UniqFile::getPath() {
    return (_path);
}

void UniqFile::write(const char* __s, std::streamsize __n) {
    _file.write(__s, __n);
}

void UniqFile::read(char* __s, std::streamsize __n) {
    _file.read(__s, __n);
}
