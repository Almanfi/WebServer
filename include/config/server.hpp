/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:43:49 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/09 18:47:09 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "configFlag.hpp"

class Server {
public:
    vector<string>  server_name;
    string  listenIp;
    int     listenPort;
    string  error_page;
    Server(Config& c, Parser& p);
    ~Server();
    void linkLocation();
    void setMainLocation(string& token);
    void setNewLocation();
    void setServerInfo(string& token);
    void parseListen();
    void parseServerName();
    void parseErrorPage();
    const string& validateIp(const string& ip);
    int validatePort(const string& port);
    void checkServerInfo();
    void finalize();
    void setAllowedDirective();
    map<string, Location*> locations; // uri, location
private:
    static map<string, int> directive;
    map<string, string> info;
    Config& c;
    Parser& p;
};

#endif // SERVER_HPP