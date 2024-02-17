/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:43 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/11 19:12:48 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSER_HPP
# define PARSER_HPP

# include "definition.hpp"

class Parser {
public:
    ~Parser();
    bool is_space(const char c);
    bool is_delimitter(const char c);
    std::string getToken();
    int getLineNum();
    std::vector<configScope>& getScopes();
    static Parser& getInstance();
    static void init(const std::string& filePath);
    static void destroy();
    static std::string getTok();
private:
    Parser();
    // Parser(const std::string& filePath);
    static Parser instance;
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    std::vector<configScope> scopes;
    bool isInitialized;
};

#endif // PARSER_HPP