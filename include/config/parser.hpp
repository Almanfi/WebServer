/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elasce <elasce@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 16:41:43 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/28 13:27:00 by elasce           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSER_HPP
# define PARSER_HPP

# include "definition.hpp"

class Parser {
public:
    Parser(const std::string& filePath);
    ~Parser();
    bool is_space(const char c);
    bool is_delimitter(const char c);
    std::string getToken();
    int getLineNum();
    std::vector<configScope>& getScopes();
    static Parser& getInstance();

private:
    static Parser* instance;
    std::ifstream configFile;
    int lineNumber;
    std::string line;
    size_t linePos;
    std::vector<configScope> scopes;
};

#endif // PARSER_HPP