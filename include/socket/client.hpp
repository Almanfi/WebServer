/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/11 18:07:01 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "socket.hpp"

class ServerSocket;

class ISocketManager {
public:
	virtual ~ISocketManager() {};
	virtual ssize_t send() = 0;
	virtual ssize_t recv() = 0;
};

class SocketManager : public ISocketManager {
public:
	SocketManager(const sock_fd fd, ISBuffer& buffer);
	~SocketManager();
	ssize_t send();
	ssize_t recv();
private:
	const sock_fd	_fd;
	ISBuffer&		_buffer;
};

class IResponse {
public:
    virtual ~IResponse() {};
    virtual void sendResponse() = 0;
};

class Response : public IResponse {
public:
	Response(ISBuffer& buffer, IUniqFile& file);
	void sendResponse();
private:
	ISBuffer& buffer;
	IUniqFile& file;
};


class IServerSocket;

class IClientResourceManagerFacade;

class IClientResourceManagerFactory {
public:
	virtual ~IClientResourceManagerFactory() {};
	virtual IClientResourceManagerFacade* createFacade(sock_fd fd, IServerSocket& servSock) = 0;
	virtual ISBuffer* createBuffer() = 0;
	virtual Iuuid* createUUID() = 0;
	virtual IHeader* createRequestHeader() = 0;
	virtual IUniqFile* createUniqFile(string rootPath, Iuuid& uuid) = 0;
	virtual ISocketManager* createSocketManager(sock_fd& fd, ISBuffer& buffer) = 0;
	virtual IRequest* createRequest(ISBuffer& buffer, IUniqFile& file, IHeader& headers) = 0;
	virtual Response* createResponse(ISBuffer& buffer, IUniqFile& file) = 0;
};

class ClientResourceManagerFactory : public IClientResourceManagerFactory {
	ClientResourceManagerFactory(const ClientResourceManagerFactory& other);
	ClientResourceManagerFactory& operator=(const ClientResourceManagerFactory& other);
public:
	IClientResourceManagerFacade* createFacade(sock_fd fd, IServerSocket& servSock);
	ClientResourceManagerFactory();
	~ClientResourceManagerFactory();
	ISBuffer* createBuffer();
	Iuuid* createUUID();
	IHeader* createRequestHeader();
	IUniqFile* createUniqFile(string rootPath, Iuuid& uuid);
	ISocketManager* createSocketManager(sock_fd& fd, ISBuffer& buffer);
	IRequest* createRequest(ISBuffer& buffer, IUniqFile& file, IHeader& headers);
	Response* createResponse(ISBuffer& buffer, IUniqFile& file);
};

class IClientResourceManagerFacade {
public:
	virtual ~IClientResourceManagerFacade() {};
	virtual ISocketManager&		socketManager() = 0;
    virtual IRequest&			request() = 0;
    virtual IResponse&			response() = 0;
    virtual IUniqFile&			file() = 0;
	virtual void				destroyFactory() = 0;
	virtual Iuuid&				uuid() = 0;
	virtual IServerSocket&		servSock() = 0;
};

class ClientResourceManagerFacade : public IClientResourceManagerFacade {
	IClientResourceManagerFactory* factory;
	sock_fd         _fd;
	IServerSocket*   _servSock;
	Iuuid*          _uuid;
	IUniqFile*      _file;
	ISBuffer*       _buffer;
	IHeader*		_requestHeaders;
	IRequest*       _request;
	Response*       _response;
	ISocketManager*     _socketManager;
public:
	ClientResourceManagerFacade(sock_fd fd, IServerSocket& servSock,
								IClientResourceManagerFactory* factory);
	~ClientResourceManagerFacade();
	ISocketManager&		socketManager();
    IRequest&			request();
    IResponse&			response();
    IUniqFile&			file();
	void				destroyFactory();
	Iuuid&				uuid();
	IServerSocket&		servSock();
    class ResourceException : public std::exception {
    public:
        ResourceException();
        virtual const char* what() const throw();
    };
private:
    void cleanupResources();
	void createUniqFile();
	ClientResourceManagerFacade(const ClientResourceManagerFacade& other);
	ClientResourceManagerFacade& operator=(const ClientResourceManagerFacade& other);
};

class Client {
	public:
	Client(IClientResourceManagerFacade* RMF);
	~Client();
	
	const cnx_state&    handleState();
	ssize_t     send();
	ssize_t     recieve();
	Server&     getServer();
	const Iuuid& getUUID();
	friend class Response;
private:
	Client(const Client& other);
	Client& operator=(const Client& other);
	IClientResourceManagerFacade* RMF;
	ISocketManager&  socketManager;
	IUniqFile&	file;
	IRequest&	request;
	IResponse&	response;
	cnx_state   state;
	int			statusCode;
};

#endif // CLIENT_HPP