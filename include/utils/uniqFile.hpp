/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniqFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 19:42:15 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/09 19:55:19 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIQFILE_HPP
# define UNIQFILE_HPP

# include "definition.hpp"
# include "uuid.hpp"

class IUniqFile {
public:
    virtual ~IUniqFile() {};
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void write(const char* __s, std::streamsize __n) = 0;
    virtual size_t read(char* __s, std::streamsize __n) = 0;
};

class UniqFile : public IUniqFile {
    fstream _file;
    string  _path;
    Iuuid&  _uuid;
    void    createFile();
    FILE    *_stream;
public:
    UniqFile(string rootPath, Iuuid &uuid);
    virtual ~UniqFile();
    virtual void open();
    virtual void freopen(const char* mode, std::FILE* stream );
    virtual void close();
    virtual void remove();
    virtual void write(const char* s, std::streamsize n);
    virtual size_t read(char* s, std::streamsize n);
    virtual string getPath();
    void seekg(std::streamoff off, std::ios_base::seekdir way);
    struct stat getStat();

};




#endif // UNIQFILE_HPP