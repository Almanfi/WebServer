/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/08 22:16:20 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Client::Client(IClientResourceManagerFacade* RMF,
			ISocketManager& socketManager,
			IFileManager& fileManager,
			IRequestManager& requestManager,
			IResponseManager& responseManager) :
                RMF(RMF),
                socketManager(socketManager),
                fileManager(fileManager),
                requestManager(requestManager),
                responseManager(responseManager),
                state(NONE) {
    RMF->destroyFactory();
}

Client::Client(const Client& other) : RMF(other.RMF) ,
                                      socketManager(other.socketManager),
                                      fileManager(other.fileManager),
                                      requestManager(other.requestManager),
                                      responseManager(other.responseManager),
                                      state(other.state) {
    throw std::runtime_error("Client::copy constructor: forbidden");
}

Client& Client::operator=(const Client& other) {
    (void) other;
    throw std::runtime_error("Client::operator=: forbidden");
}

Client::~Client() {
    delete RMF;
}

// sock_fd Client::getFd() {
//     return (fd);
// }

ssize_t Client::send() {
    cout << "++++++++++++ send ++++++++++++" << endl;
    responseManager.sendResponse();
    ssize_t bytes_sent = socketManager.send();
    if (bytes_sent == -1) {
        perror("send");
        throw std::exception();
    }
    state = CLOSE;
    return (bytes_sent);
}

ssize_t Client::recieve() {
    cout << "++++++++++++ recieve ++++++++++++" << endl;
    ssize_t bytes_received = socketManager.recv();
    fileManager.openFile();
    if (requestManager.parse() == true)
        state = WRITE;
    fileManager.closeFile();
    cout << "++++++++++++ recieve end ++++++++++++" << endl;
    return (bytes_received);
}

const cnx_state& Client::handleState() {
    if (state == NONE)
        state = READ;
    switch (state) {
        case READ:
            if (recieve() == 0)
                state = CLOSE;
            break ;
        case WRITE:
            send();
            break ;
        case CLOSE:
            cout << "++++++++++++ closing ++++++++++++" << endl;
            break ;
        default:
            throw std::runtime_error("Client::handle: invalid state");
    }
    return (state);
}

// void Client::readBuffer() {
//     cout    << "Received request: " << endl
//             << buffer << std::endl;
// }

// cnx_state& Client::getState() {
//     return (state);
// }

Server& Client::getServer() {
    ServerSocket& servSock = RMF->servSock();
    return (*servSock.getServers()[0]);
}

const Iuuid& Client::getUUID() {
    return (RMF->uuid());
}


// socketManager

// ClientResourceManager

ClientResourceManager::ClientResourceManager(sock_fd fd, ServerSocket& servSock) :
            _fd(fd),
            _servSock(servSock),
            _state(NONE),
            _uuid(NULL),
            _file(NULL),
            _buffer(NULL),
            _request(NULL),
            _response(NULL),
            _socketManager(NULL),
            _fileManager(NULL),
            _requestManager(NULL),
            _responseManager(NULL) {
}

ClientResourceManager::ClientResourceManager(const ClientResourceManager& other) : 
            _fd(other._fd),
            _servSock(other._servSock) {
    throw std::runtime_error("ClientResourceManager::copy constructor: forbidden");
}

ClientResourceManager& ClientResourceManager::operator=(const ClientResourceManager& other) {
    (void) other;
    throw std::runtime_error("ClientResourceManager::operator=: forbidden");
}

ClientResourceManager::~ClientResourceManager() {
    cout << "cleaning resource of client " << endl;
    if (_uuid)
        delete _uuid;
    if (_file)
        delete _file;
    if (_buffer)
        delete _buffer;
    if (_response)
        delete _response;
    if (_request)
        delete _request;
    if (_socketManager)
        delete _socketManager;
    if (_fileManager)
        delete _fileManager;
    if (_requestManager)
        delete _requestManager;
    if (_responseManager)
        delete _responseManager;
    if (_requestHeaders)
        delete _requestHeaders;
}

Iuuid& ClientResourceManager::uuid() {
    if (!_uuid)
        _uuid = new UUID();
    return *_uuid;
}

IuniqFile& ClientResourceManager::file(string rootPath) {
    if (!_file)
        _file = new UniqFile(rootPath, uuid());
    return *_file;
}

IuniqFile& ClientResourceManager::file() {
    if (!_file)
        throw std::runtime_error("ClientResourceManager::file: file not initialized");
    return *_file;
}

ISBuffer& ClientResourceManager::buffer() {
    if (!_buffer)
        _buffer = new SBuffer();
    return *_buffer;
}

Response& ClientResourceManager::response() {
    if (!_response)
        _response = new Response(buffer(), file());
    return *_response;
}

IRequest& ClientResourceManager::request() {
    if (!_requestHeaders)
        _requestHeaders = new Header();
    if (!_request)
        _request = new Request(buffer(), file(), *_requestHeaders);
    return *_request;
}

ServerSocket& ClientResourceManager::servSock() {
    return _servSock;
}

sock_fd& ClientResourceManager::fd() {
    return _fd;
}

cnx_state& ClientResourceManager::state() {
    return _state;
}

ISocketManager& ClientResourceManager::socketManager() {
    if (!_socketManager)
        _socketManager = new SocketManager(fd(), buffer());
    return *_socketManager;
}

IFileManager& ClientResourceManager::fileManager() {
    if (!_fileManager)
        _fileManager = new FileManager(file());
    return *_fileManager;
}

IRequestManager& ClientResourceManager::requestManager() {
    if (!_requestManager)
        _requestManager = new RequestManager(request());
    return *_requestManager;
}

IResponseManager& ClientResourceManager::responseManager() {
    if (!_responseManager)
        _responseManager = new ResponseManager(response());
    return *_responseManager;
}

// SocketManager

SocketManager::SocketManager(sock_fd& fd, ISBuffer& buffer) : _fd(fd), _buffer(buffer) {}

SocketManager::~SocketManager() {
    close(_fd);
}

ssize_t SocketManager::send() {
    return _buffer.send(_fd, 0); // TODO check flags later
	// return ::send(_fd, &_buffer, _buffer.size(), 0);
}

ssize_t SocketManager::recv() {
    ssize_t bytes_received = _buffer.recv(_fd, 0); // TODO check flags later
    cout << "bytes_received = " << bytes_received << endl;
    cout << "buffer = " << dynamic_cast<SBuffer&>(_buffer) << endl;
    return bytes_received;
	// return ::recv(_fd, &_buffer, _buffer.size(), 0); // TODO check flags later
}

// FileManager

FileManager::FileManager(IuniqFile& file) : file(file) {}



void FileManager::openFile() {
	file.openFile();
}

void FileManager::closeFile() {
	file.closeFile();
}

// RequestManager

RequestManager::RequestManager(IRequest& request) :
				request(request) {
}

bool RequestManager::parse() {
	return (request.parse());
}

string RequestManager::getHeader(const string& key) {
	return (request.getHeader(key));
}

// Response

Response::Response(ISBuffer& buffer, IuniqFile& file) :
				buffer(buffer), file(file) {}

void Response::sendResponse() {
	(void) file;
	buffer.clear();
    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    // std::string response = "HTTP/1.1 300\r\nLocation: https://www.google.com/\r\n\r\n";
    buffer.write(response);// TODO check size
	// std::memcpy(&buffer, response.c_str(), response.size());
}

// ResponseManager

ResponseManager::ResponseManager(Response& response) :
				response(response) {}

void ResponseManager::sendResponse() {
	response.sendResponse();
}


// ResourceManagerFactory

ClientResourceManagerFactory::ClientResourceManagerFactory() {}

ClientResourceManagerFactory::~ClientResourceManagerFactory() {}

ClientResourceManagerFactory::ClientResourceManagerFactory(const ClientResourceManagerFactory& other) {
	(void)other;
	throw std::runtime_error("ClientResourceManagerFactory is not copyable");
}

ClientResourceManagerFactory& ClientResourceManagerFactory::operator=(const ClientResourceManagerFactory& other) {
	(void)other;
	throw std::runtime_error("ClientResourceManagerFactory is not copyable");
}

IClientResourceManagerFacade* ClientResourceManagerFactory::createFacade(sock_fd fd, ServerSocket& servSock) {
	return new ClientResourceManagerFacade(fd, servSock, this);
}

ISBuffer* ClientResourceManagerFactory::createBuffer() {
	return new SBuffer();
}

Iuuid* ClientResourceManagerFactory::createUUID() {
	return new UUID();
}

IHeader* ClientResourceManagerFactory::createRequestHeader() {
    return new Header();
}

IuniqFile* ClientResourceManagerFactory::createUniqFile(string rootPath, Iuuid& uuid) {
	return new UniqFile(rootPath, uuid);
}

ISocketManager* ClientResourceManagerFactory::createSocketManager(sock_fd& fd, ISBuffer& buffer) {
	return new SocketManager(fd, buffer);
}

IFileManager* ClientResourceManagerFactory::createFileManager(IuniqFile& file) {
	return new FileManager(file);
}

IRequestManager* ClientResourceManagerFactory::createRequestManager(IRequest& request) {
	return new RequestManager(request);
}

IResponseManager* ClientResourceManagerFactory::createResponseManager(Response& response) {
	return new ResponseManager(response);
}

IRequest* ClientResourceManagerFactory::createRequest(ISBuffer& buffer, IuniqFile& file, IHeader& headers) {
	return new Request(buffer, file, headers);
}

Response* ClientResourceManagerFactory::createResponse(ISBuffer& buffer, IuniqFile& file) {
	return new Response(buffer, file);
}


// ResourceManagerFacade


ClientResourceManagerFacade::ClientResourceManagerFacade (
							sock_fd fd,
							ServerSocket& servSock,
							IClientResourceManagerFactory* factory) :
							_fd(fd),
							_servSock(servSock),
							_uuid(NULL),
							_file(NULL),
							_buffer(NULL),
                            _requestHeaders(NULL),
							_request(NULL),
							_response(NULL),
							_socketManager(NULL),
							_fileManager(NULL),
							_requestManager(NULL),
							_responseManager(NULL),
							factory(factory) {
}

ClientResourceManagerFacade::~ClientResourceManagerFacade() {
    delete _uuid;
    delete _file;
    delete _buffer;
    delete _requestHeaders;
    delete _request;
    delete _response;
    delete _socketManager;
    delete _fileManager;
    delete _requestManager;
    delete _responseManager;
    delete factory;
}

ClientResourceManagerFacade::ClientResourceManagerFacade(const ClientResourceManagerFacade& other) :
							_servSock(other._servSock) {
	(void)other;
	throw std::runtime_error("ClientResourceManagerFacade is not copyable");
}

ClientResourceManagerFacade& ClientResourceManagerFacade::operator=(const ClientResourceManagerFacade& other) {
	(void)other;
	throw std::runtime_error("ClientResourceManagerFacade is not copyable");
}

ISocketManager& ClientResourceManagerFacade::socketManager() {
	if (!_socketManager) {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_buffer)
			_buffer = factory->createBuffer();
		_socketManager = factory->createSocketManager(_fd, *_buffer);
	}
	return *_socketManager;
}

IFileManager& ClientResourceManagerFacade::fileManager() {
	if (!_fileManager) {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_file)
			createUniqFile();
		_fileManager = factory->createFileManager(*_file);
	}
	return *_fileManager;
}

IRequestManager& ClientResourceManagerFacade::requestManager() {
	if (!_requestManager)  {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_buffer)
			_buffer = factory->createBuffer();
		if (!_file)
			createUniqFile();
        if (!_requestHeaders)
            _requestHeaders = factory->createRequestHeader();
		if (!_request)
			_request = factory->createRequest(*_buffer, *_file, *_requestHeaders);
		_requestManager = factory->createRequestManager(*_request);
	}
	return *_requestManager;
}

IResponseManager& ClientResourceManagerFacade::responseManager() {
	if (!_responseManager) {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_buffer)
			_buffer = factory->createBuffer();
		if (!_file)
			createUniqFile();
		if (!_response)
			_response = factory->createResponse(*_buffer, *_file);
		_responseManager = factory->createResponseManager(*_response);
	}
	return *_responseManager;
}

void ClientResourceManagerFacade::destroyFactory() {
	delete factory;
	factory = NULL;
}

void ClientResourceManagerFacade::createUniqFile() {
	if (!factory)
		throw std::runtime_error("factory is not set");
	if (!_uuid)
		_uuid = factory->createUUID();
	if (!_file)
		_file = factory->createUniqFile("tmp", *_uuid);
}

Iuuid& ClientResourceManagerFacade::uuid() {
    if (!_uuid) {
        if (!factory)
            throw std::runtime_error("factory is not set");
        _uuid = factory->createUUID();
    }
    return *_uuid;
}

ServerSocket& ClientResourceManagerFacade::servSock() {
    return _servSock;
}