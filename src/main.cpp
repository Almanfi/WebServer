/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/12 16:30:16 by maboulkh         ###   ########.fr       */
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
    Epoll epoll(conf);
    epoll.loop();
    
    return 0;
}
