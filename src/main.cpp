/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 20:00:49 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include "config.hpp"
#include "socket.hpp"
#include <algorithm>
#include <signal.h>

#include <sys/epoll.h>


int main() {
    signal(SIGPIPE, SIG_IGN);
    try
    {
        Config& conf = Config::init("config/default.conf");
        // conf.print();
        //// -- cout<< "type is " << Config::getMimeType("html");

        cout <<  "by default php and py are deault in a location" << endl;
        cout <<  "but if specified only the specified one will be allowed" << endl;
        IClientConf& loc = conf.getLocation("localhost/test");
        cout << "cgi allowed : " << loc.allowCGI() << endl;
        cout << "cgi timeout : " << loc.CGITimeout() << endl;
        cout << "cgi_allowed : " << loc.isCgiFile("/cgi-bin/echo.py") << endl;
        cout << "cgi executable : " << loc.cgiExecutable("/cgi-bin/echo.py") << endl;
        cout << "cgi_allowed : " << loc.isCgiFile("/cgi-bin/echo.php") << endl;
        cout << "cgi executable : " << loc.cgiExecutable("/cgi-bin/echo.php") << endl;

        Epoll epoll;
        epoll.init(conf);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
