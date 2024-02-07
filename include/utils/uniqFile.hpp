/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniqFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 19:42:15 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/07 00:01:54 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIQFILE_HPP
# define UNIQFILE_HPP

# include "definition.hpp"
# include "uuid.hpp"

class IuniqFile {
public:
    virtual ~IuniqFile() {};
    virtual void openFile() = 0;
    virtual void closeFile() = 0;
    virtual void write(const char* __s, std::streamsize __n) = 0;
    virtual void read(char* __s, std::streamsize __n) = 0;
    // virtual void deleteFile() = 0;
};

class UniqFile : public IuniqFile {
    fstream _file;
    string  _path;
    Iuuid&  _uuid;
    void    createFile();
public:
    UniqFile(string rootPath, Iuuid &uuid);
    virtual ~UniqFile();
    virtual void openFile();
    virtual void closeFile();
    virtual void deleteFile();
    virtual string getPath();
    virtual void write(const char* __s, std::streamsize __n);
    virtual void read(char* __s, std::streamsize __n);
};




#endif // UNIQFILE_HPP