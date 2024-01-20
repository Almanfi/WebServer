/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:44:46 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/20 03:37:00 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "definition.hpp"

class Location {
public:
    Location(Config& c, Server& serv, Parser& p, const string& uri);
    ~Location();
    void set();
    void setLocationInfo(string& token);
    void addToInLoc(Location* loc);
    void propagate();
    string& getUri() const;
    void print(int space);
    void printThis(int space);
    static void setAlloedDirective();
    string getInfo(const string& key);
    Location& getLocation(const string& uri);
private:
    void setNewLoc();
    void checkLocationInfo();
    map<string, string> info;
    deque<Location*> inLoc;
    static map<string, int> directive;
    Config& c;
    Server& serv;
    Parser& p;
    string uri;
};

#endif // LOCATION_HPP