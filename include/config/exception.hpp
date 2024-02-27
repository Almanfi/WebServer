/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 19:07:12 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 20:17:36 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

#include "definition.hpp"
#include "parser.hpp"

class ConfigException {
protected:
    static string getLineNum();
public:
    class DIRECT_NOT_VALID: public std::exception {
        string msg;
    public :
        ~DIRECT_NOT_VALID() throw();
        DIRECT_NOT_VALID(const string& directive);
        virtual const char* what() const throw();
    };
    class MISSING_BRACKET: public std::exception {
        string msg;
    public :
        ~MISSING_BRACKET() throw();
        MISSING_BRACKET(const string& bracket);
        virtual const char* what() const throw();
    };
    class DIRECT_ALREADY_SET: public std::exception {
        string msg;
    public :
        ~DIRECT_ALREADY_SET() throw();
        DIRECT_ALREADY_SET(const string& directive);
        virtual const char* what() const throw();
    };
    class TOO_MANY_ARGS: public std::exception {
        string msg;
    public :
        ~TOO_MANY_ARGS() throw();
        TOO_MANY_ARGS(const string& directive);
        virtual const char* what() const throw();
    };
};

class  ServerException : public ConfigException {
public:
    class NOT_VALID_PORT: public std::exception {
        string msg;
    public :
        ~NOT_VALID_PORT() throw();
        NOT_VALID_PORT(const string& port);
        virtual const char* what() const throw();
    };
    class LISTEN_NOT_FOUND: public std::exception {
        string msg;
    public :
        ~LISTEN_NOT_FOUND() throw();
        LISTEN_NOT_FOUND();
        virtual const char* what() const throw();
    };
    class SERVER_NAME_NOT_FOUND: public std::exception {
        string msg;
    public :
        ~SERVER_NAME_NOT_FOUND() throw();
        SERVER_NAME_NOT_FOUND(const string& serverName);
        virtual const char* what() const throw();
    };
    class MISSING_LOCATION_URI: public std::exception {
        string msg;
    public :
        ~MISSING_LOCATION_URI() throw();
        MISSING_LOCATION_URI();
        virtual const char* what() const throw();
    };
    class INVALID_LOCATION_URI: public std::exception {
        string msg;
    public :
        ~INVALID_LOCATION_URI() throw();
        INVALID_LOCATION_URI(const string& uri);
        virtual const char* what() const throw();
    };
};

class  LocationException : public ServerException {
public:
    class INVALID_METHOD: public std::exception {
        string msg;
    public :
        ~INVALID_METHOD() throw();
        INVALID_METHOD(const string& method);
        virtual const char* what() const throw();
    };
    class INVALID_ROOT: public std::exception {
        string msg;
    public :
        ~INVALID_ROOT() throw();
        INVALID_ROOT(const string& root);
        virtual const char* what() const throw();
    };
    class INVALID_VALUE: public std::exception {
        string msg;
    public :
        ~INVALID_VALUE() throw();
        INVALID_VALUE(const string& key, const string& value);
        virtual const char* what() const throw();
    };
    class INVALID_PATH: public std::exception {
        string msg;
    public :
        ~INVALID_PATH() throw();
        INVALID_PATH(const string& path);
        virtual const char* what() const throw();
    };
    class MISSING_CGI_PATH: public std::exception {
        string msg;
    public :
        ~MISSING_CGI_PATH() throw();
        MISSING_CGI_PATH(const string& cgi_uri);
        virtual const char* what() const throw();
    };
    
};

#endif // EXCEPTION_HPP