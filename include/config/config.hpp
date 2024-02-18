/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/18 03:57:37 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "definition.hpp"
# include "parser.hpp"
# include "server.hpp"
# include "location.hpp"

class Config {
public:
    ~Config();
    void read();
    void print();
    void setAlloedDirective();
    Location& getLocation(const string& uri);
    deque<Server>& getServers();
    static const string getDefault(const string& key);
    static Config& getInstance();
    static Config& init(const string& filePath);
    static void destroy();
    
private:
    Config();
    void initRules();
    static Config instance;
    KeyVal defaultConfig;
    void set(const string& token);
    void setServer();
    void readMainContext();
    deque<Server> servers;
    static map<string, int> directive;
    bool isInitialized;
};

class ConfigException {
protected:
    static string getLineNum() {
        stringstream ss;
        ss << Parser::getInstance().getLineNum();
        return ss.str();
    }
public:
    class DIRECT_NOT_VALID: public std::exception {
        string msg;
    public :
        ~DIRECT_NOT_VALID() throw() {}
        DIRECT_NOT_VALID(const string& directive) {
            msg = "Error: directive `" + directive
                + "` not valid at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class MISSING_BRACKET: public std::exception {
        string msg;
    public :
        ~MISSING_BRACKET() throw() {}
        MISSING_BRACKET(const string& bracket) {
            msg = "Error: Missing ";
            if (bracket == "{")
                msg += "opening ";
            else if (bracket == "}")
                msg += "closing ";
            msg += "bracket at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class DIRECT_ALREADY_SET: public std::exception {
        string msg;
    public :
        ~DIRECT_ALREADY_SET() throw() {}
        DIRECT_ALREADY_SET(const string& directive) {
            msg = "Error: directive `" + directive
                + "` already set at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class TOO_MANY_ARGS: public std::exception {
        string msg;
    public :
        ~TOO_MANY_ARGS() throw() {}
        TOO_MANY_ARGS(const string& directive) {
            msg = "Error: too many arguments for directive `"
                + directive + "` at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
};

class  ServerException : public ConfigException {
public:
    class NOT_VALID_PORT: public std::exception {
        string msg;
    public :
        ~NOT_VALID_PORT() throw() {}
        NOT_VALID_PORT(const string& port) {
            msg = "Error: port `" + port
                + "` not valid at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class LISTEN_NOT_FOUND: public std::exception {
        string msg;
    public :
        ~LISTEN_NOT_FOUND() throw() {}
        LISTEN_NOT_FOUND() {
            msg = "Error: directive `listen` not found at line "
                + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class SERVER_NAME_NOT_FOUND: public std::exception {
        string msg;
    public :
        ~SERVER_NAME_NOT_FOUND() throw() {}
        SERVER_NAME_NOT_FOUND(const string& serverName) {
            msg = "Error: server `" + serverName + "` not found";
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class MISSING_LOCATION_URI: public std::exception {
        string msg;
    public :
        ~MISSING_LOCATION_URI() throw() {}
        MISSING_LOCATION_URI() {
            msg = "Error: missing location URI at line "
                + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class INVALID_LOCATION_URI: public std::exception {
        string msg;
    public :
        ~INVALID_LOCATION_URI() throw() {}
        INVALID_LOCATION_URI(const string& uri) {
            msg = "Error: location URI `" + uri
                + "` not valid at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
};

class  LocationException : public ServerException {
public:
    class INVALID_METHOD: public std::exception {
        string msg;
    public :
        ~INVALID_METHOD() throw() {}
        INVALID_METHOD(const string& method) {
            msg = "Error: method `" + method
                + "` is not supported at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class INVALID_ROOT: public std::exception {
        string msg;
    public :
        ~INVALID_ROOT() throw() {}
        INVALID_ROOT(const string& root) {
            msg = "Error: root `" + root
                + "` not valid at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class INVALID_VALUE: public std::exception {
        string msg;
    public :
        ~INVALID_VALUE() throw() {}
        INVALID_VALUE(const string& key, const string& value) {
            msg = "Error: value `" + value + "` not valid for the "
                + key  + " directive at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class INVALID_PATH: public std::exception {
        string msg;
    public :
        ~INVALID_PATH() throw() {}
        INVALID_PATH(const string& path) {
            msg = "Error: path `" + path
                + "` not valid at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    class MISSING_CGI_PATH: public std::exception {
        string msg;
    public :
        ~MISSING_CGI_PATH() throw() {}
        MISSING_CGI_PATH(const string& cgi_uri) {
            msg = "Error: cgi_path for `" + cgi_uri
                + "` is missing at line " + getLineNum();
        }
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
    
};

#endif // CONFIG_HPP