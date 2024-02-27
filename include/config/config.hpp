/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/27 19:12:16 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "definition.hpp"
# include "exception.hpp"
# include "parser.hpp"
# include "server.hpp"
# include "location.hpp"

class Config {
public:
    ~Config();
    void read();
    void print();
    void setAlloedDirective();
    Location& getLocation(const string& uri);
    deque<Server>& getServers();
    static const string getDefault(const string& key);
    static Config& getInstance();
    static Config& init(const string& filePath);
    static void destroy();
    static const string& getMimeType(const string& ext);
    static const string& getStatusCode(int code);
    
private:
    Config();
    void initRules();
    void initMime();
    void initStatusCodes();
    static Config instance;
    KeyVal defaultConfig;
    void set(const string& token);
    void setServer();
    void readMainContext();
    deque<Server> servers;
    static map<string, int> directive;
    static map<string, string> mime_types;
    static map<int, string> status_codes; 
    bool isInitialized;
};

#endif // CONFIG_HPP