/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 17:34:16 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/10 19:00:29 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include "config/config.hpp"


int main() {
    Config conf("config/default.config");
    conf.read();
    conf.print();
    cout << "-------------------------------------" << endl;
    cout << "config for server 1" << endl;
    Location& loc = conf.getLocation("server22/a/bb/cdlksfjdlsj/dsfklj/");
    cout << "server name: " << loc.getInfo("server_name") << endl;
    cout << "listen: " << loc.getInfo("listen") << endl;
    cout << "error_page: " << loc.getInfo("error_page") << endl;
    loc.printThis(0);
    return 0;
}
