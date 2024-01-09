/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:43 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/09 16:44:16 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSER_HPP
# define PARSER_HPP

# include "configFlag.hpp"

class Parser {
public:
    Parser(const std::string& filePath);
    ~Parser();
    bool findBracket(const std::string& bracket);
    bool is_space(const char c);
    bool is_delimitter(const char c);
    bool isEndingLine();
    std::string getToken();
    int getLineNum();
    std::vector<configScope>& getScopes();

private:
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    std::vector<configScope> scopes;
};

#endif // PARSER_HPP