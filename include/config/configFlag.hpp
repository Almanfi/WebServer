/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFlag.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:24:48 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/09 18:41:37 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFLAG_HPP
# define CONFIGFLAG_HPP

// # define CONTEXT
#define MAIN_CONTEXT 0x1
#define SERVER_CONTEXT 0x2
#define LOCATION_CONTEXT 0x4
#define HTTP_CONTEXT 0x8
#define EVENTS_CONTEXT 0x10

#define MUST_HAVE_ONE 0x20
#define MUST_HAVE 0x40

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <cstdlib>

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
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

class Parser;
class Server;
class Location;
class Config;


#endif // CONFIGFLAG_HPP