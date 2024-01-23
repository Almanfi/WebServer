/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   definition.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:44:54 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/23 16:11:34 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINITION_HPP
# define DEFINITION_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <ctime>
# include <algorithm>
# include <vector>
# include <deque>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/epoll.h>
# include <sys/stat.h>
# include <netdb.h>
# include <errno.h>

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::fstream;
using std::stringstream;
using std::string;
using std::map;
using std::vector;
using std::deque;

typedef enum e_configScope {
    MAIN,
    SERVER,
    LOCATION
} configScope;

typedef enum {
    NONE,
    READ,
    WRITE,
    ERROR,
    CLOSE
}   cnx_state;

class Parser;
class Server;
class Location;
class Config;

#define MAX_LISTEN 10 // max number of clients in the queue
#define RECIEVE_MAX_SIZE 4096

#define S_HOST "host"
#define S_PORT "port"

typedef Location ClientConf;
typedef map<string, string>  KeyVal;
typedef int sock_fd;


#endif // DEFINITION_HPP