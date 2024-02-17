/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:44:46 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/12 19:12:01 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "definition.hpp"

class IClientConf {
public:
    virtual ~IClientConf() {};
    virtual string getErrorPage(const string& code) = 0;
    virtual string getErrorPage(const int code) = 0;
    virtual bool isAllowedMethod(const string& method) = 0;
    virtual string getInfo(const string& key) = 0;
};

class Location  : public IClientConf {
public:
    Location(const string& uri);
    // Location(const Location& loc);
    ~Location();
    void setServer(Server& servRef);
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
    static void initValidationMap();

    string getErrorPage(const string& code);
    string getErrorPage(const int code);
    bool isAllowedMethod(const string& method);
private:
    void validateDirective(const string& key, const string& value);
    void validateRoot(const string& value);
    void validateIndex(const string& value);
    void validateAutoindex(const string& value);
    void validateClientMaxBodySize(const string& value);
    void validateCgi(const string& value);
    void validateMethods(const string& value);
    void validateReturn(const string& value);
    void validateErrorPage(const string& value);
    void insertDirective(const string& key, const string& value);

    void setNewLoc();
    map<string, string> info;
    // deque<Location*> inLoc;
    static map<string, int> directive;
    static vector<string> httpAllowedMethods;
    static map<string, void (Location::*) (const string&)> validationMap;
    map<string, Location> innerLocations;
    string uri;
    Server* serv;
};

#endif // LOCATION_HPP