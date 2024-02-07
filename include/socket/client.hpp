/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 23:14:40 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/07 01:25:15 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "socket.hpp"

class ServerSocket;

// class Client {
//     public:
//     Client(sock_fd fd, ServerSocket& servSock);
//     Client(const Client& other);
//     Client& operator=(const Client& other);
//     ~Client();
	
//     const cnx_state&    handleState();
//     ssize_t     send();
//     ssize_t     recieve();
//     void        readBuffer();
//     sock_fd     getFd();
//     Server&     getServer();
//     const UUID& getUUID();
//     void        createFile();
//     void        openFile();
//     friend class Response;
// private:
//     sock_fd     fd;
//     cnx_state   state;
//     UUID        uuid;
//     fstream     file;
//     SBuffer     buffer;
//     ServerSocket& servSock;
//     Request     request;
//     Response    response;
//     string      data;
// };

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
	// Client(sock_fd fd, ServerSocket& servSock);
	// Client(IClientResourceManager* resourceManager);
	Client(IClientResourceManager* resourceManager,
			ISocketManager& socketManager,
			IFileManager& fileManager,
			IRequestManager& requestManager,
			IResponseManager& responseManager);
	~Client();
	
	const cnx_state&    handleState();
	ssize_t     send();
	ssize_t     recieve();
	// void        readBuffer();
	// sock_fd     getFd();
	Server&     getServer();
	const Iuuid& getUUID();
	// void        createFile();
	// void        openFile();
	friend class Response;
private:
	Client(const Client& other);
	Client& operator=(const Client& other);
	IClientResourceManager* RM;
	ISocketManager&  socketManager;
	IFileManager& 	 fileManager;
	IRequestManager& requestManager;
	IResponseManager& responseManager;
	cnx_state   state;
	// sock_fd     fd;
	// UUID        uuid;
	// fstream     file;
	// SBuffer     buffer;
	// ServerSocket& servSock;
	// Request     request;
	// Response    response;
	// string      data;
};

#endif // CLIENT_HPP