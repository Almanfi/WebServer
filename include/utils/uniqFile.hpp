/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniqFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 19:42:15 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 14:55:34 by maboulkh         ###   ########.fr       */
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
    virtual void read(char* __s, std::streamsize __n) = 0;
    virtual string getPath() = 0;
};

class UniqFile : public IUniqFile {
    fstream _file;
    string  _path;
    Iuuid&  _uuid;
    void    createFile();
public:
    UniqFile(string rootPath, Iuuid &uuid);
    virtual ~UniqFile();
    virtual void open();
    virtual void close();
    virtual void remove();
    virtual void write(const char* __s, std::streamsize __n);
    virtual void read(char* __s, std::streamsize __n);
    virtual string getPath();
};




#endif // UNIQFILE_HPP