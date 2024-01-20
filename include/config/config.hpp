/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 03:36:21 by maboulkh         ###   ########.fr       */
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
    Config(const string& filePath);
    ~Config();
    void read();
    deque<Location>& getLocations();
    void print();
    void setAlloedDirective();
    Location& getLocation(const string& uri);
    deque<Server>& getServers();
private:
    void set(const string& token);
    void setServer();
    void readMainContext();
    Parser p;
    deque<Server> servers;
    deque<Location> locations;
    static map<string, int> directive;
};

class locExp {
public:
    class DIRECT_NOT_VALID: public std::exception {
        virtual const char* what() const throw() {
            return "Error: directive not valid";
        }
    };
    class TOO_MANY_ARGS: public std::exception {
        virtual const char* what() const throw() {
            return "Error: too many arguments";
        }
    };
    class DIRECT_ALREADY_SET: public std::exception {
        virtual const char* what() const throw() {
            return "Error: directive already set";
        }
    };
    class NOT_VALID_PORT: public std::exception {
        virtual const char* what() const throw() {
            return "Error: not valid port";
        }
    };
    class LISTEN_NOT_FOUND: public std::exception {
        virtual const char* what() const throw() {
            return "Error: no listen";
        }
    };
};

#endif // CONFIG_HPP