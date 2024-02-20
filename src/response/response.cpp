#include "socket.hpp"

Response::Response(IHeader &requestHeaders, IUniqFile &file, IClientConf *config, int fd) : started(false), ended(false), reachedEOF(false), isCGIStarted(false), isCGIEnded(false),
                                                                                            requestHeaders(requestHeaders), body(file),
                                                                                            config(config), fd(fd)
{
}

Response::~Response()
{
}

void Response::uriParser()
{
    std::string tmp = this->uri;
    std::string::size_type pos = tmp.find("?");
    if (pos != std::string::npos)
    {
        this->query = tmp.substr(pos + 1);
        this->uri = tmp.substr(0, pos);
    }
    else
        this->query = "";
}

void Response::initResponse(IClientConf *conf,int status_code)
{
    // this->fd = 0;  // TODO fd
    cout << "++++++++++++ initResponse ++++++++++++" << endl;
    config = conf;
    this->method = requestHeaders.getMethod();
    this->uri = decodingURI(requestHeaders.getUri());
    uriParser();
    std::cout << "uri: " << uri << std::endl;
    this->bodyPath = body.getPath();
    this->locationPath = joinPath(config->root(), uri);
    std::cout << "----------- uri: " << uri << std::endl;   
    std::cout << "----------- root: " << config->root() << std::endl;
    std::cout << "++++++++++ locationPath: " << locationPath << std::endl;
    this->status_code = status_code;
}

void Response::sendResponse()
{
    if (!started)
        cout << "++++++++++++ sendResponse ++++++++++++" << endl;
    if(this->status_code != 200)
        handleError(this->status_code);
    if (isForCGI())
        handleCGI();
    else if (!checkForValidMethod())
        handleError(405);
    else if (config->returnCode() != 0)
        handleRedirection();
    else
    {
        if (method == GET)
            handleGet();
        else if (method == POST)
            handlePost();
        else if (method == DELETE)
            handleDelete();
    }
}

bool Response::isForCGI()
{
    struct stat buff;
   if(!config->allowCGI())
        return false;
    if (stat(locationPath.c_str(), &buff) != 0)
    {
        if (errno == ENOENT)
            handleError(404);
        else if (errno == EACCES)
            handleError(403);
        else
            handleError(500);
    }
    if(S_ISDIR(buff.st_mode))
        return false;
    return true;
}

void Response::handleGet()
{
    cout << "++++++++++++ handleGet ++++++++++++" << endl;
    std::cout << "locationPath: " << locationPath << std::endl;
    if (stat((locationPath).c_str(), &buff) != 0)
    {
        std::cout << "Get stat errno: " << errno << std::endl;
        if (errno == ENOENT)
            handleError(404);
        else if (errno == EACCES)
            handleError(403);
        else
            handleError(500);
        return;
    }
    if (S_ISDIR(buff.st_mode))
        handleDirectory();
    else if (S_ISREG(buff.st_mode))
        handleFile();
    else
        handleError(404);
}

void Response::handlePost()
{
    std::cout << "++++++++++++ handlePost ++++++++++++" << std::endl;
    std::string newPath = joinPath(config->uploadPath(), uri);
    std::string oldPath = body.getPath();
    if (config->allowUpload())
    {
        if (rename(oldPath.c_str(), newPath.c_str()) != 0)
        {
            if (errno == ENOENT)
                handleError(404);
            else if (errno == EACCES)
                handleError(403);
            else
                handleError(500);
            return;
        }
        header.setStatusCode(201);
        header.setConnection("close");
        header.setContentType("application/octet-stream");
        header.setContentLength(0);
        bufferToSend = header.getHeader();
        sendNextChunk();
        if (bufferToSend.size() == 0)
            ended = true;
        return;
    }
    else
        handleError(403);
}
void Response::handleDelete()
{
    std::cout << "++++++++++++ handleDelete ++++++++++++" << std::endl;
    std::string pathToDelete = joinPath(config->uploadPath(), uri);
    if (config->allowUpload())
    {
        if (stat(pathToDelete.c_str(), &buff) != 0)
        {
            if (errno == ENOENT)
                handleError(404);
            else if (errno == EACCES)
                handleError(403);
            else
                handleError(500);
            return;
        }
        if (S_ISDIR(buff.st_mode))
            return handleError(403);
        remove(pathToDelete.c_str());
        return;
    }
    header.setStatusCode(204);
    header.setConnection("close");
    header.setContentType("application/octet-stream");
    header.setContentLength(0);
    bufferToSend = header.getHeader();
    sendNextChunk();
    if (bufferToSend.size() == 0)
        ended = true;
    return;
}

void Response::handleDirectory()
{
    std::cout << "++++++++++++ handleDirectory ++++++++++++" << std::endl;
    for (size_t i = 0; i < config->index().size(); i++)
    {
        std::string path = joinPath(locationPath, config->index()[i]);
        std::cout << "handleDirectory path: " << path << std::endl;
        if (stat(path.c_str(), &buff) == 0 && S_ISREG(buff.st_mode))
        {
            sendFile(path);
            return;
        }
    }
    if (config->autoindex())
        sendDirectory(locationPath);
    else
        handleError(403);
}
void Response::handleFile()
{
    sendFile(locationPath);
}

void Response::sendFile(const std::string &path)
{
    int size = 0;
    std::cout << "++++++++++++++++++++++ in sendFile ++++++++++++++++++++++++++++" << std::endl;
    if (!started)
    {
        header.setStatusCode(200);
        header.setConnection("close");
        header.setContentType(path);
        header.setContentLength(buff.st_size);
        bufferToSend = header.getHeader();
        fileToSend.open(path.c_str(), std::ios::in | std::ios::out | std::ios::app);
        if (!fileToSend.is_open())
        {
            if (errno == EACCES)
                return handleError(403);
            else
                return handleError(500);
        }
        started = true;
    }
    if (!reachedEOF)
    {
        size = bufferToSend.size();
        char tmpBuffer[1024];
        fileToSend.read(tmpBuffer, 1024 - size);
        size_t readSize = fileToSend.gcount();
        bufferToSend.append(tmpBuffer, readSize);
        if (fileToSend.eof())
        {
            reachedEOF = true;
            fileToSend.close();
        }
    }
    sendNextChunk();
    if (bufferToSend.size() == 0 && reachedEOF)
        ended = true;
}

void Response::sendNextChunk()
{
    int sendedSize = 0;
    sendedSize = send(fd, bufferToSend.c_str(), bufferToSend.size(), 0);
    if (sendedSize == -1)
    {
        perror("send");
        ended = true;
    }
    bufferToSend.erase(0, sendedSize);
}

void Response::handleError(int error_code)
{
    std::string errorPage;
    if (config->getErrorPage(error_code) != "")
    {
        std::string path = joinPath(locationPath, config->getErrorPage(error_code));
        if (stat(path.c_str(), &buff) == 0 && S_ISREG(buff.st_mode))
        {
            sendFile(path);
            return;
        }
    }
    errorPage = generateErrorPage(error_code, header.getStatusMessage(error_code));
    if (!isHeaderSent)
    {
        header.setStatusCode(error_code);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setContentLength(errorPage.size());
        bufferToSend = header.getHeader();
        started = true;
        isHeaderSent = true;
    }

    bufferToSend.append(errorPage);
    sendNextChunk();
    if (bufferToSend.size() == 0)
        ended = true;
}

bool Response::isEnded()
{
    return ended;
}

bool Response::isStarted()
{
    return started;
}
void Response::sendDirectory(const std::string &path)
{
    std::cout << "++++++++++++ sendDirectory ++++++++++++" << std::endl;
    std::cout << "Directory path: " << path << std::endl;
    DIR *dir = NULL;

    std::string listingPageHTML;

    if (!started)
    {
        dir = opendir(path.c_str());
        if (!dir)
        {
            if (errno == EACCES)
                return handleError(403);
            else
                return handleError(500);
        }

        listingPageHTML = generateDirectoryListingPage(dir, uri, locationPath);
        header.setStatusCode(301);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setContentLength(listingPageHTML.size());
        bufferToSend = header.getHeader();

        started = true;
    }

    bufferToSend.append(listingPageHTML);
    sendNextChunk();
    if (bufferToSend.size() == 0)
        ended = true;
    closedir(dir);
}

bool Response::checkForValidMethod()
{
    for (size_t i = 0; i < config->methods().size(); i++)
    {
        if (config->methods()[i] == convertT_method(method))
            return true;
    }
    return false;
}

bool Response::handleRedirection()
{
    std::cout << "++++++++++++ handleRedirection ++++++++++++" << std::endl;
    if (config->returnCode() == 301 || config->returnCode() == 302 || config->returnCode() == 303 || config->returnCode() == 307 || config->returnCode() == 308)
    {
        header.setStatusCode(config->returnCode());
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setHeader("Location", config->returnUrl());
        bufferToSend = header.getHeader();
        sendNextChunk();
        if (bufferToSend.size() == 0)
            ended = true;
        return true;
    }
    else
    {
        header.setStatusCode(config->returnCode());
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "application/octet-stream");
        header.setContentLength(config->returnUrl().size());
        bufferToSend = header.getHeader();
        bufferToSend.append(config->returnUrl());
        sendNextChunk();
        if (bufferToSend.size() == 0)
            ended = true;
        return true;
    }
    return false;
}
