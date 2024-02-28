/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:44:46 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/28 02:00:26 by maboulkh         ###   ########.fr       */
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
    
    virtual vector<string>  methods() = 0;
    virtual string          root() = 0;
    virtual vector<string>  index() = 0;
    virtual bool            autoindex() = 0;
    virtual int             returnCode() = 0;
    virtual string          returnUrl() = 0;
    virtual bool            allowUpload() = 0;
    virtual string          uploadPath() = 0;
    virtual bool            allowCGI() = 0;
    virtual string          CGIPath() = 0;
    virtual size_t          CGITimeout() = 0;
    virtual int             getPort() = 0;
    virtual bool            isCgiFile(const string& path) = 0;
    virtual const string    cgiExecutable(const string& path) = 0;
    virtual size_t          clientTimeout() = 0;
    virtual size_t          clientMaxBodySize() = 0;
};

class Location  : public IClientConf {
public:
    Location(const string& uri);
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

    virtual vector<string>  methods();
    virtual string          root();
    virtual vector<string>  index();
    virtual bool            autoindex();
    virtual int             returnCode();
    virtual string          returnUrl();
    virtual bool            allowUpload();
    virtual string          uploadPath();
    virtual bool            allowCGI();
    virtual string          CGIPath();
    virtual size_t          CGITimeout();
    virtual int             getPort();
    virtual bool            isCgiFile(const string& path);
    virtual const string    cgiExecutable(const string& path);
    virtual size_t          clientTimeout();
    virtual size_t          clientMaxBodySize();
private:
    void validateDirective(const string& key, const string& value);
    void validateRoot(const string& value);
    void validateIndex(const string& value);
    void validateAutoindex(const string& value);
    void validateClientMaxBodySize(const string& value);
    void validateCgi(const string& value);
    void validateCgiPath(const string& value);
    void validateCgiTimeout(const string& value);
    void validateMethods(const string& value);
    void validateReturn(const string& value);
    void validateErrorPage(const string& value);
    void insertDirective(const string& key, const string& value);
    void validateUpload(const string& value);
    void validateUploadPath(const string& value);
    void validateAllowedCgi(const string& value);
    void validateClientTimeout(const string& value);
    
    

    void setNewLoc();
    map<string, string> info;
    static map<string, int> directive;
    static vector<string> httpAllowedMethods;
    static map<string, void (Location::*) (const string&)> validationMap;
    map<string, Location> innerLocations;
    string uri;
    Server* serv;
};

#endif // LOCATION_HPP