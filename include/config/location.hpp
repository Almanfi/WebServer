/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:44:46 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/21 17:54:14 by maboulkh         ###   ########.fr       */
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
    deque<Location*> inLoc;
    static map<string, int> directive;
    static vector<string> httpAllowedMethods;
    static map<string, void (Location::*) (const string&)> validationMap;
    Config& c;
    Server& serv;
    Parser& p;
    string uri;
};

#endif // LOCATION_HPP