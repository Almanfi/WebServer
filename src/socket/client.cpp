/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/17 18:57:15 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Client::Client(IClientResourceManagerFacade* RMF) :
                RMF(RMF),
                socketManager(RMF->socketManager()),
                file(RMF->file()),
                request(RMF->request()),
                response(RMF->response()),
                state(NONE), statusCode(200) {
}

Client::Client(const Client& other) : RMF(other.RMF) ,
                                      socketManager(other.socketManager),
                                      file(other.file),
                                      request(other.request),
                                      response(other.response),
                                      state(other.state),
                                      statusCode(other.statusCode) {
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

// ssize_t Client::send() {
//     cout << "++++++++++++ send ++++++++++++" << endl;
//     response.sendResponse();
//     ssize_t bytes_sent = socketManager.send();
//     if (bytes_sent == -1) {
//         perror("send");
//         throw std::exception();
//     }
//     state = CLOSE;
//     return (bytes_sent);
// }

ssize_t Client::send() {
    ssize_t bytes_sent = 0;
    if(!response.isStarted())
        response.initResponse(this);
    response.sendResponse();
    // sendFile(fd, "./tmp/en.subject.pdf");
    if(response.isEnded())
        state = CLOSE;
    return (bytes_sent);
}


ssize_t Client::recieve() {
    cout << "++++++++++++ recieve ++++++++++++" << endl;
    ssize_t bytes_received = socketManager.recv();
    cout << "bytes_received = " << bytes_received << endl;
    if (bytes_received == 0)
        state = CLOSE;
    file.open();
    try {
        if (request.parse() == true) {
            state = WRITE;
            RMF->destroyRequest();
        }
    }
    catch (const RequestException::BAD_REQUEST& e) {
        state = WRITE;
        statusCode = 400;
    }
    catch (const RequestException::REQUEST_HEADER_FIELDS_TOO_LARGE& e) {
        state = WRITE;
        statusCode = 431;
    }
    catch (const RequestException::NOT_IMPLIMENTED& e) {
        state = WRITE;
        statusCode = 501;
    }
    catch (const RequestException::HTTP_VERSION_NOT_SUPPORTED& e) {
        state = WRITE;
        statusCode = 505;
    }
    catch (const RequestException::LENGTH_REQUIRED& e) {
        state = WRITE;
        statusCode = 411;
    }
    catch (const std::exception& e) {
        state = WRITE;
        statusCode = 500;
    }
    file.close();
    cout << "++++++++++++ recieve end ++++++++++++" << endl;
    return (bytes_received);
}

const cnx_state& Client::handleState() {
    if (state == NONE)
        state = READ;
    switch (state) {
        case READ:
            recieve();
            break ;
        case WRITE:
            send();
            break ;
        case CLOSE:
            break ;
        default:
            statusCode = 500;
            state = WRITE;
            break;
    }
    return (state);
}

Server& Client::getServer() {
    IServerSocket& servSock = RMF->servSock();
    return (*servSock.getServers()[0]);
}

const Iuuid& Client::getUUID() {
    return (RMF->uuid());
}

// SocketManager

SocketManager::SocketManager(const sock_fd fd, ISBuffer& buffer) : _fd(fd), _buffer(buffer) {}

SocketManager::~SocketManager() {
    close(_fd);
}

ssize_t SocketManager::send() {
    return _buffer.send(_fd, 0); // TODO check flags later
}

ssize_t SocketManager::recv() {
    ssize_t bytes_received = _buffer.recv(_fd, 0); // TODO check flags later
    cout << "bytes_received = " << bytes_received << endl;
    cout << "buffer = " << dynamic_cast<SBuffer&>(_buffer) << endl;
    return bytes_received;
	// return ::recv(_fd, &_buffer, _buffer.size(), 0); // TODO check flags later
}

// ResponseB

ResponseB::ResponseB(ISBuffer& buffer, IUniqFile& file) :
				buffer(buffer), file(file) {}

void ResponseB::sendResponse() {
	(void) file;
	buffer.clear();
    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    // std::string response = "HTTP/1.1 300\r\nLocation: https://www.google.com/\r\n\r\n";
    buffer.write(response);// TODO check size
	// std::memcpy(&buffer, response.c_str(), response.size());
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

IClientResourceManagerFacade* ClientResourceManagerFactory::createFacade(sock_fd fd, IServerSocket& servSock) {
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

IUniqFile* ClientResourceManagerFactory::createUniqFile(string rootPath, Iuuid& uuid) {
	return new UniqFile(rootPath, uuid);
}

ISocketManager* ClientResourceManagerFactory::createSocketManager(sock_fd& fd, ISBuffer& buffer) {
	return new SocketManager(fd, buffer);
}

IRequest* ClientResourceManagerFactory::createRequest(ISBuffer& buffer, IUniqFile& file, IHeader& headers,
							IServerSocket& servSock, IClientConf* config) {
	return new Request(buffer, file, headers, servSock, config);
}

Response* ClientResourceManagerFactory::createResponse(
    IHeader& requestHeaders, IUniqFile& file, IClientConf& config, int fd) {
	return new Response(requestHeaders, file, config, fd);
}

// ResourceManagerFacade

ClientResourceManagerFacade::ResourceException::ResourceException() {}

const char* ClientResourceManagerFacade::ResourceException::what() const throw() {
    return ("ResourceException");
}

ClientResourceManagerFacade::ClientResourceManagerFacade (
							sock_fd fd,
							IServerSocket& servSock,
							IClientResourceManagerFactory* factory) :
                                factory(factory), _fd(fd), _servSock(&servSock),
                                _uuid(NULL), _file(NULL), _buffer(NULL),
                                _requestHeaders(NULL), _request(NULL),
                                _response(NULL), _socketManager(NULL),
                                _config(NULL) {
    _uuid = factory->createUUID();
    _file = factory->createUniqFile("tmp", *_uuid);
    _buffer = factory->createBuffer();
    _requestHeaders = factory->createRequestHeader();
    _request = factory->createRequest(*_buffer, *_file, *_requestHeaders
                                    , servSock, _config);
    _response = factory->createResponse(*_requestHeaders, *_file, *_config, _fd);
    _socketManager = factory->createSocketManager(_fd, *_buffer);
    if (!_buffer || !_uuid || !_file
        || !_requestHeaders || !_request
        || !_response || !_socketManager) {
        cleanupResources();
        throw ResourceException();
    }
}

ClientResourceManagerFacade::~ClientResourceManagerFacade() {
    cleanupResources();
}

void ClientResourceManagerFacade::cleanupResources() {
    delete _uuid; _uuid = NULL;
    delete _file; _file = NULL;
    delete _buffer; _buffer = NULL;
    delete _requestHeaders; _requestHeaders = NULL;
    delete _request; _request = NULL;
    delete _response; _response = NULL;
    delete _socketManager; _socketManager = NULL;
    delete factory; factory = NULL;
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

IUniqFile& ClientResourceManagerFacade::file() {
	if (!_file)
		createUniqFile();
	return *_file;
}

IRequest& ClientResourceManagerFacade::request() {
	if (!_request)  {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_buffer)
			_buffer = factory->createBuffer();
		if (!_file)
			createUniqFile();
        if (!_requestHeaders)
            _requestHeaders = factory->createRequestHeader();
		_request = factory->createRequest(*_buffer, *_file, *_requestHeaders, *_servSock, _config);
	}
	return *_request;
}

Response& ClientResourceManagerFacade::response() {
	if (!_response) {
		if (!factory)
			throw std::runtime_error("factory is not set");
		if (!_buffer)
			_buffer = factory->createBuffer();
        if (!_config)
            throw std::runtime_error("config is not set");
		if (!_file)
			createUniqFile();
        if (!_requestHeaders) {
            _requestHeaders = factory->createRequestHeader();
        }
	    _response = factory->createResponse(*_requestHeaders, *_file, *_config, _fd);
	}
	return *_response;
}

void ClientResourceManagerFacade::destroyFactory() {
	delete factory;
	factory = NULL;
}

void ClientResourceManagerFacade::destroyRequest() {
    delete _request;
    _request = NULL;
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

IServerSocket& ClientResourceManagerFacade::servSock() {
    return *_servSock;
}

IClientConf* ClientResourceManagerFacade::configRef() {
    return _config;
}

IClientConf& ClientResourceManagerFacade::config() {
    if (!_config)
        throw std::runtime_error("config is not set");
    return *_config;
}