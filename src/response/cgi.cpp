#include "socket.hpp"


char **Response::getEnvironmentVariables()
{
    env["AUTH_TYPE"] = "";
    // env["CONTENT_LENGTH"] = ToString(this->bodyStat.st_size);
    // env["CONTENT_TYPE"] = request->getHeader("Content-Type");// TODO I changed this
    env["CONTENT_TYPE"] = requestHeaders.getHeader("Content-Type");
    env["REDIRECT_STATUS"] = "200";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["PATH_INFO"] = this->locationPath;
    env["PATH_TRANSLATED"] = this->locationPath;
    env["QUERY_STRING"] = this->query;
    env["REMOTE_ADDR"] = "";  // need to be change later
    env["REMOTE_HOST"] = "";  // need to be change later
    env["REMOTE_IDENT"] = ""; // need to be change later
    env["REMOTE_USER"] = "";  // need to be change later
    // env["REQUEST_METHOD"] = t_methodToString(request->headers.method); // TODO
    env["SCRIPT_NAME"] = this->locationPath;
    env["SERVER_NAME"] = "Webserv";
    env["SERVER_PORT"] = "9992";
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    char **env = new char *[this->env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = this->env.begin(); it != this->env.end(); it++)
    {
        std::string tmp = it->first + "=" + it->second;
        env[i] = new char[tmp.size() + 1];
        strcpy(env[i], tmp.c_str());
        i++;
    }
    env[i] = NULL;
    return env;
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

// --------------- using tmpfile -----------------//

void Response::initCGI()
{
    cgiOutPutFile = "./tmp/" + generateRandomFileName("cgi", ".cgi");
    cgiFd[1] = open(cgiOutPutFile.c_str(), O_CREAT | O_RDWR, 0666);
    if (cgiFd[1] == -1)
    {
        std::cerr << "Error Opening file Output: " << strerror(errno) << std::endl;
        return;
    }
    cgiFd[0] = open((bodyPath).c_str(), O_RDONLY);
    if (cgiFd[0] == -1)
    {
        std::cerr << "Error Opening file Input: " << strerror(errno) << std::endl;
        return;
    }
    cgiStartTimer = std::clock();
    cgiPid = fork();
    if (cgiPid == -1)
    {
        std::cerr << "Error forking: " << strerror(errno) << std::endl;
        return;
    }
    if (cgiPid == 0)
    {
        if (dup2(cgiFd[0], STDIN_FILENO) == -1 || dup2(cgiFd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "Error duplicating file descriptors: " << strerror(errno) << std::endl;
            exit(1);
        }
        char *args[3] = {strdup(config->CGIPath().c_str()), strdup(locationPath.c_str()), NULL};
        char **env = getEnvironmentVariables();
        int ret = execve(config->CGIPath().c_str(), args, env);
        if (ret == -1)
        {
            std::cerr << "Error executing CGI: " << strerror(errno) << std::endl;
            exit(1);
        }
    }
    else
    {
        isCGIStarted = true;
        close(cgiFd[0]);
        close(cgiFd[1]);
    }
}

bool Response::checkGGIProcess()
{
    if (isCGIEnded)
        return false;
    if (cgiPid < 0)
    {
        std::cerr << "Error forking: " << strerror(errno) << std::endl;
        cgiStatus = 500;
        isCGIEnded = true;
        return false;
    }
    else
    {
        int processStatus;
        int res = waitpid(cgiPid, &processStatus, WNOHANG);
        if (res == -1)
        {
            std::cerr << "Error waiting for CGI: " << strerror(errno) << std::endl;
            cgiStatus = 500;
            isCGIEnded = true;
            return false;
        }
        else if (res == 0)
        {
            // char *end;
            std::clock_t now = std::clock();
            size_t timeLimit;
            // double timeLimit = std::stod(config->CGITimeout(), &end);
            if(config->CGITimeout() < 10)
                timeLimit = config->CGITimeout();
            else
                timeLimit = 10;
            if (static_cast<double>(now - cgiStartTimer) / CLOCKS_PER_SEC > timeLimit)
            {
                std::cerr << "CGI timed out" << std::endl;
                kill(cgiPid, SIGKILL);
                cgiStatus = 500;
                isCGIEnded = true;
                return false;
            }
            else
                return true;
        }
        else
        {
            if (WIFEXITED(processStatus))
            {
                std::cout << "CGI exited normally" << std::endl;
                // if (WEXITSTATUS(processStatus) == 0)
                // {
                //     std::cout << "CGI exited with status 0" << std::endl;
                //     cgiStatus = 200;
                // }
                // else
                // {
                //     std::cerr << "CGI exited with status " << WEXITSTATUS(processStatus) << std::endl;
                //     cgiStatus = 500;
                // }
                cgiStatus = WEXITSTATUS(processStatus);
                isCGIEnded = true;
                return false;
            }
            else
            {
                std::cerr << "CGI exited abnormally" << std::endl;
                cgiStatus = 500;
                isCGIEnded = true;
                return false;
            }
        }
    }
    return true;
}

void Response::handleCGI()
{
    started = true;
    if (!isCGIStarted)
        initCGI();
    if (checkGGIProcess())
        return;
    if (isCGIEnded)
    {
        if (cgiStatus == 500)
        {
            remove(cgiOutPutFile.c_str());
            handleError(500);
        }
        else
            handleCGIResponse();
    }
}

std::string Response::extractCGIHeaders()
{
    std::ifstream file(cgiOutPutFile.c_str());
    std::string line;
    std::string headerKey;
    std::string headerValue;
    std::string headers;
    while (std::getline(file, line))
    {
        if (line == "\n" || line == "\r\n" || line == "\r" || line == "")
            break;
        std::string::size_type pos = line.find(":");
        if (pos != std::string::npos)
        {
            headerKey = line.substr(0, pos);
            headerValue = line.substr(pos + 1);
            cgiHeaderSize += line.size();
            std::cout << "extract Header: " << headerKey << " : " << headerValue << std::endl;
            this->header.setCGIHeader(headerKey, headerValue);
            headerValue.clear();
            headerKey.clear();
        }
    }
    return this->header.getCGIHeader();
}

void Response::handleCGIResponse()
{
    if (!isHeaderSent)
    {
        std::string headers = extractCGIHeaders();
        std::cout << "-----------------------Headers: " << headers << "------------------------------" << std::endl;
        std::cout << "Headers: " << headers << std::endl;
        int contentLength = 0;
        struct stat statbuf;
        if (stat(cgiOutPutFile.c_str(), &statbuf) == -1)
        {
            std::cerr << "Error getting file stats: " << strerror(errno) << std::endl;
            handleError(500);
            return;
        }
        contentLength = statbuf.st_size - cgiHeaderSize;
        std::cout << "Header size: " << cgiHeaderSize << std::endl;
        std::cout << "File size: " << statbuf.st_size << std::endl;
        std::cout << "Content Length: " << contentLength << std::endl;

        bufferToSend = headers;
        header.setContentLength(contentLength);
        fileToSend.open(cgiOutPutFile.c_str(), std::ios::in);
        if (!fileToSend.is_open())
        {
            std::cerr << "Error opening output File: " << strerror(errno) << std::endl;
            handleError(500);
            return;
        }
        fileToSend.seekg(cgiHeaderSize);
        isHeaderSent = true;
    }
    if (!reachedEOF)
    {
        int size = bufferToSend.size();
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
    {
        ended = true;
        remove(cgiOutPutFile.c_str());
    }
}