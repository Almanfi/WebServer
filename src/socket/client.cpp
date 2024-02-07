/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:38:36 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/07 01:36:24 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

// Client::Client(sock_fd fd, ServerSocket& servSock) :
//                                                     fd(fd),
//                                                     state(NONE),
//                                                     uuid(),
//                                                     servSock(servSock) {
//     while (access((string("/tmp/") + uuid.getStr()).c_str(), F_OK) == 0) {
//         uuid.regen();
//     }
//     createFile();
//     buffer.clear();
// }

// Client::Client(IClientResourceManager* resourceManage) : RM(resourceManage) {
// }

Client::Client(IClientResourceManager* resourceManager,
			ISocketManager& socketManager,
			IFileManager& fileManager,
			IRequestManager& requestManager,
			IResponseManager& responseManager) :
                RM(resourceManager),
                socketManager(socketManager),
                fileManager(fileManager),
                requestManager(requestManager),
                responseManager(responseManager),
                state(NONE) {
}        

Client::Client(const Client& other) : RM(other.RM) , 
                                      socketManager(other.socketManager),
                                      fileManager(other.fileManager),
                                      requestManager(other.requestManager),
                                      responseManager(other.responseManager),
                                      state(other.state) {
    throw std::runtime_error("Client::copy constructor: forbidden");
}

// Client::Client(const Client& other) :
//                                     fd(other.fd),
//                                     state(other.state),
//                                     uuid(other.uuid),
//                                     buffer(other.buffer),
//                                     servSock(other.servSock),
//                                     request(other.request) {           
// }

Client& Client::operator=(const Client& other) {
    (void) other;
    throw std::runtime_error("Client::operator=: forbidden");
}
// Client& Client::operator=(const Client& other) {
//     if (this != &other) {
//         fd = other.fd;
//         state = other.state;
//         uuid = other.uuid;
//         buffer = other.buffer;
//         request = other.request;
//     }
//     return (*this);
// }

Client::~Client() {
    delete RM;
    // if (file.is_open())
    //     file.close();
    // // remove((string("./tmp/") + uuid.getStr()).c_str()); // TODO uncomment
    // close(fd);
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
    // ssize_t bytes_sent = 0;
    // if(!response.isStarted())
    //     response.initResponse(this);
    // response.sendResponse();
    // // sendFile(fd, "./tmp/en.subject.pdf");
    // cnx_state state = RM->state();
    // if(response.isEnded())
    //     state = CLOSE;
    // return (bytes_sent);
}

ssize_t Client::recieve() {
    cout << "++++++++++++ recieve ++++++++++++" << endl;
    ssize_t bytes_received = socketManager.recv();
    fileManager.openFile();
    // RM->file().openFile();
    // ISBuffer& buffer = RM->buffer();
    // IuniqFile& file = RM->file();
    // Request& request = RM->request();
    // ssize_t bytes_received = RM->buffer().recv(RM->fd(), 0); // TODO check flags later
    if (requestManager.parse() == true)
        state = WRITE;
    // if (request.parseRequest(buffer, file) == true)
    //     state = WRITE;
    fileManager.closeFile();
    // file.close();
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
    ServerSocket& servSock = RM->servSock();
    return (*servSock.getServers()[0]);
}

// void Client::createFile() {
//     file.open((string("./tmp/") + uuid.getStr()).c_str(), std::ios::in | std::ios::out | std::ios::trunc);
//     if (!file.is_open()) {
//         perror("open");
//         throw std::exception();
//     }
//     file.close();
// }

// void Client::openFile() {
//     file.open((string("./tmp/") + uuid.getStr()).c_str(), std::ios::in | std::ios::out | std::ios::app);
//     if (!file.is_open()) {
//         perror("open");
//         throw std::exception();
//     }
// }

const Iuuid& Client::getUUID() {
    return (RM->uuid());
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
            _responseManager(NULL){
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
    if (!_request)
        _request = new Request(buffer(), file());
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
