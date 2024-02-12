/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:43:49 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/12 15:54:44 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "definition.hpp"
# include "location.hpp"

class Location;

class Server {
public:
    vector<string>  server_name;
    string  listenIp;
    int     listenPort;
    string  error_page;
    Server();
    // Server(const Server& serv);
    ~Server();
    void set();
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
    string getInfo(const string& key) const;
    Location& getLocation(const string& uri);
    Location& getRootLocation();
    // map<string, Location*> locations; // uri, location
private:
    // map<string, Location> locations;
    static map<string, int> directive;
    map<string, string> info;
    Location rootLocation;
};

#endif // SERVER_HPP