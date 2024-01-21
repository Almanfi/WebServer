/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/21 18:14:33 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include "config.hpp"
#include "socket.hpp"
#include <algorithm>


#include <sys/epoll.h>


int main() {
    Config conf("config/default.config");
    conf.read();
    // conf.print();

    // Location& loc = conf.getLocation("127.0.0.3/a/b");
    // loc.print(0);
    // cout << "error page is " << loc.getErrorPage("401") << endl;
    // string method = "DELETE";
    // cout << "method " << method << " is allowed " << loc.isAllowedMethod(method) << endl;
    
    Epoll epoll(conf);
    epoll.loop();
    
    return 0;
}
