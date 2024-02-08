/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/08 22:17:10 by maboulkh         ###   ########.fr       */
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
	SocketManager(sock_fd& fd, ISBuffer& buffer);
	~SocketManager();
	ssize_t send();
	ssize_t recv();
private:
	sock_fd&    _fd;
	ISBuffer&   _buffer;
};

class IFileManager {
public:
	virtual ~IFileManager() {};
	virtual void openFile() = 0;
	virtual void closeFile() = 0;
};

class FileManager : public IFileManager {
public:
	FileManager(IuniqFile& file);
	void openFile();
	void closeFile();
private:
	IuniqFile& file;
};

class IRequestManager {
public:
	virtual ~IRequestManager() {};
	virtual bool parse() = 0;
	virtual string getHeader(const string& key) = 0;
};

class RequestManager : public IRequestManager {
public:
	RequestManager(IRequest& request);
	virtual bool parse();
	virtual string getHeader(const string& key);
private:
	IRequest&   request;
};

class IResponseManager {
public:
	virtual ~IResponseManager() {};
	virtual void sendResponse() = 0;
};

class Response {
public:
	Response(ISBuffer& buffer, IuniqFile& file);
	void sendResponse();
private:
	ISBuffer& buffer;
	IuniqFile& file;
};




class ResponseManager : public IResponseManager {
public:
	ResponseManager(Response& response);
	void sendResponse();
private:
	Response& response;
};

class IClientResourceManager;

class ClientResourceManager;

class IClientResourceManagerFacade;

class IClientResourceManagerFactory {
public:
	virtual ~IClientResourceManagerFactory() {};
	virtual IClientResourceManagerFacade* createFacade(sock_fd fd, ServerSocket& servSock) = 0;
	virtual ISBuffer* createBuffer() = 0;
	virtual Iuuid* createUUID() = 0;
	virtual IHeader* createRequestHeader() = 0;
	virtual IuniqFile* createUniqFile(string rootPath, Iuuid& uuid) = 0;
	virtual ISocketManager* createSocketManager(sock_fd& fd, ISBuffer& buffer) = 0;
	virtual IFileManager* createFileManager(IuniqFile& file) = 0;
	virtual IRequestManager* createRequestManager(IRequest& request) = 0;
	virtual IResponseManager* createResponseManager(Response& response) = 0;
	virtual IRequest* createRequest(ISBuffer& buffer, IuniqFile& file, IHeader& headers) = 0;
	virtual Response* createResponse(ISBuffer& buffer, IuniqFile& file) = 0;
};

class ClientResourceManagerFactory : public IClientResourceManagerFactory {
	ClientResourceManagerFactory(const ClientResourceManagerFactory& other);
	ClientResourceManagerFactory& operator=(const ClientResourceManagerFactory& other);
public:
	IClientResourceManagerFacade* createFacade(sock_fd fd, ServerSocket& servSock);
	ClientResourceManagerFactory();
	~ClientResourceManagerFactory();
	ISBuffer* createBuffer();
	Iuuid* createUUID();
	IHeader* createRequestHeader();
	IuniqFile* createUniqFile(string rootPath, Iuuid& uuid);
	ISocketManager* createSocketManager(sock_fd& fd, ISBuffer& buffer);
	IFileManager* createFileManager(IuniqFile& file);
	IRequestManager* createRequestManager(IRequest& request);
	IResponseManager* createResponseManager(Response& response);
	IRequest* createRequest(ISBuffer& buffer, IuniqFile& file, IHeader& headers);
	Response* createResponse(ISBuffer& buffer, IuniqFile& file);
};

class IClientResourceManagerFacade {
public:
	virtual ~IClientResourceManagerFacade() {};
	virtual ISocketManager&		socketManager() = 0;
	virtual IFileManager&		fileManager() = 0;
	virtual IRequestManager&	requestManager() = 0;
	virtual IResponseManager&	responseManager() = 0;
	virtual void				destroyFactory() = 0;
	virtual Iuuid&				uuid() = 0;
	virtual ServerSocket&		servSock() = 0;
};

class ClientResourceManagerFacade : public IClientResourceManagerFacade {
	sock_fd         _fd;
	ServerSocket&   _servSock;
	Iuuid*          _uuid;
	IuniqFile*      _file;
	ISBuffer*       _buffer;
	IHeader*		_requestHeaders;
	IRequest*       _request;
	Response*       _response;
	ISocketManager*     _socketManager;
	IFileManager*       _fileManager;
	IRequestManager*    _requestManager;
	IResponseManager*	_responseManager;
	IClientResourceManagerFactory* factory;
public:
	ClientResourceManagerFacade(sock_fd fd, ServerSocket& servSock,
								IClientResourceManagerFactory* factory);
	~ClientResourceManagerFacade();
	ISocketManager&		socketManager();
	IFileManager&		fileManager();
	IRequestManager&	requestManager();
	IResponseManager&	responseManager();
	void				destroyFactory();
	Iuuid&				uuid();
	ServerSocket&		servSock();
private:
	void createUniqFile();
	ClientResourceManagerFacade(const ClientResourceManagerFacade& other);
	ClientResourceManagerFacade& operator=(const ClientResourceManagerFacade& other);
};

class IClientResourceManager {
public:
	virtual ~IClientResourceManager() {};
	virtual sock_fd& fd() = 0;
	virtual ServerSocket& servSock() = 0;
	virtual cnx_state& state() = 0;
	virtual Iuuid& uuid() = 0;
	virtual IuniqFile& file(string rootPath) = 0;
	virtual IuniqFile& file() = 0;
	virtual ISBuffer& buffer() = 0;
	virtual IRequest& request() = 0;
	virtual Response& response() = 0;
	virtual ISocketManager&     socketManager() = 0;
	virtual IFileManager&       fileManager() = 0;
	virtual IRequestManager&    requestManager() = 0;
	virtual IResponseManager&   responseManager() = 0;
};

class ClientResourceManager : public IClientResourceManager {
	sock_fd         _fd;
	ServerSocket&   _servSock;
	cnx_state       _state;
	Iuuid*          _uuid;
	IuniqFile*      _file;
	ISBuffer*        _buffer;
	IHeader*		_requestHeaders;
	IRequest*        _request;
	Response*       _response;
	ISocketManager*     _socketManager;
	IFileManager*       _fileManager;
	IRequestManager*    _requestManager;
	IResponseManager*	_responseManager;
	ClientResourceManager();
	ClientResourceManager(const ClientResourceManager& other);
	ClientResourceManager& operator=(const ClientResourceManager& other);
public:
	ClientResourceManager(sock_fd fd, ServerSocket& servSock);
	~ClientResourceManager();
	sock_fd& fd();
	ServerSocket& servSock();
	cnx_state& state();
	Iuuid& uuid();
	IuniqFile& file(string rootPath);
	IuniqFile& file();
	ISBuffer& buffer();
	IRequest& request();
	Response& response();
	ISocketManager&     socketManager();
	IFileManager&       fileManager();
	IRequestManager&    requestManager();
	IResponseManager&   responseManager();
};

class Client {
	public:
	Client(IClientResourceManager* resourceManager,
			ISocketManager& socketManager,
			IFileManager& fileManager,
			IRequestManager& requestManager,
			IResponseManager& responseManager);
	Client(IClientResourceManagerFacade* RM,
			ISocketManager& socketManager,
			IFileManager& fileManager,
			IRequestManager& requestManager,
			IResponseManager& responseManager);
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
	IFileManager& 	 fileManager;
	IRequestManager& requestManager;
	IResponseManager& responseManager;
	cnx_state   state;
};

#endif // CLIENT_HPP