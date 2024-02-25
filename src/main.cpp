/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdiraa <fdiraa@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/25 13:13:27 by fdiraa           ###   ########.fr       */
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

        // Location& loc = conf.getLocation("127.0.0.3/a/b");
        // loc.print(0);
        //// -- cout << "error page is " << loc.getErrorPage("401") << endl;
        // string method = "DELETE";
        //// -- cout << "method " << method << " is allowed " << loc.isAllowedMethod(method) << endl;
        
        Epoll epoll;
        epoll.init(conf);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
