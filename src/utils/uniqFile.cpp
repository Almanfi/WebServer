/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniqFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 00:02:23 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/09 19:55:27 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uniqFile.hpp"

UniqFile::UniqFile(string rootPath, Iuuid &uuid) : _path(rootPath), _uuid(uuid) {
    while (access((rootPath + "/" + _uuid.getStr()).c_str(), F_OK) == 0) {
        _uuid.regen();
    }
    _path = rootPath + "/" + _uuid.getStr();
    createFile(); // TODO canot use open here? and see if file doesn t exist to create it??
}

UniqFile::~UniqFile() {
    close();
    remove();
}

void UniqFile::createFile() {
    _file.open(_path.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
    if (!_file.is_open()) {
        perror("open");
        throw std::exception();
    }
    _file.close();
}

void UniqFile::open() {
    _file.open(_path.c_str(), std::ios::in | std::ios::out | std::ios::app);
    _file.seekg(0, std::ios::beg);
    // if (!_file.is_open()) {
    //     createFile(); // TODO this create a problem when the file is already created
    // }
}



void UniqFile::remove() {
    // _file.close();
    // remove(_path.c_str()); // TODO delete file when client disconnect
}

string UniqFile::getPath() {
    return (_path);
}

void UniqFile::write(const char* __s, std::streamsize __n) {
    cout << "writing to file" << endl; // TODO remove cout.
    cout.write(__s, __n); // TODO remove cout.
    cout << endl << "-----------------" << endl; // TODO remove cout.
    _file.write(__s, __n);
}

size_t UniqFile::read(char* __s, std::streamsize __n) {
    if (!_file.is_open())
        std::cerr << "file not open" << std::endl;
    cout << "path : " << _path << endl; // TODO remove cout.
    _file.read(__s, __n);
    if (errno) {
        perror("read");
       exit(1);
    }
    return (_file.gcount());
    
}

void UniqFile::freopen(const char* mode, std::FILE* stream ) {
    _stream = std::freopen(_path.c_str(), mode, stream);
    if (_stream == NULL) {
        perror("freopen");

        throw std::exception();
    }
}

void UniqFile::close() {
    if (_stream) {
        std::fclose(_stream); 
        _stream = NULL;
    }
    _file.close();
}
void UniqFile::seekg(std::streamoff off, std::ios_base::seekdir way) {
    _file.seekg(off, way);
}

struct stat UniqFile::getStat() {
    struct stat buff;
    stat(_path.c_str(), &buff);
    return (buff);
}