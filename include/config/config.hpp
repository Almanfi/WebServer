/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 15:44:31 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/04 15:45:11 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include " configFlag.hpp"

class server {
    public:
        server();
        ~server();
        void set_listen();
        void set_server_name();
        void set_error_page();
        void set_location();
        void set_root();
        void set_index();
        void set_autoindex();
        void set_client_max_body_size();
        void set_cgi();
        void set_upload_store();
        void set_upload_pass();
        void set_upload_pass_args();
        void set_methods();
        void set_return();
        void set_auth_basic();
        void set_auth_basic_user_file();
    private:
    
};

class listen {
    public:
        listen();
        ~listen();
    private:
    
};


class NginxConfig {
public:
    NginxConfig();
    ~NginxConfig();

    // Directives
    void server();
    void listen();
    void location();
    void root();
    void index();
    void errorPage();
    // Add more directives as needed

private:
    // Private members and helper functions
    
};

#endif // CONFIG_HPP