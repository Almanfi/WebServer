#include "socket.hpp"

// static std::string getExtension(std::string path)
// {
//     for (int i = path.size() - 1; i >= 0; i--)
//     {
//         if (path[i] == '.')
//             return path.substr(i + 1);
//     }
//     return "";
// }

// static std::string ToString(int n)
// {
//     std::stringstream ss;
//     ss << n;
//     return ss.str();
// }
static std::string t_methodToString(t_method method)
{
    if (method == GET)
        return "GET";
    else if (method == POST)
        return "POST";
    else 
        return "DELETE";
}
/*
step 1: Determine CGI Path
step 2: Set Up Environment Variables
step 3: Create Input/Output Channels
step 4: Fork a Child Process
step 5: Redirect Standard I/O
step 6: Execute CGI Script
step 7: Handle CGI Script Output
step 8: Parse CGI Script Output
step 9: Construct HTTP Response
step 10: Send HTTP Response to Client
step 11: Clean Up
step 12: Handle Errors

*/

std::string generateRandomFileName(const std::string& prefix, const std::string& extension) {
      std::srand(static_cast<unsigned int>(std::time(0)));

    // Generate a random number to append to the prefix
    int randomValue = std::rand();

    // Convert the random number to a string using std::stringstream
    std::stringstream ss;
    ss << randomValue;
    std::string fileName = prefix + ss.str() + extension;

    return fileName;
}

char **Response::getEnvironmentVariables()
{
    env["AUTH_TYPE"] = "";
    // env["CONTENT_LENGTH"] = ToString(this->bodyStat.st_size);
    env["CONTENT_TYPE"] = request->getHeader("Content-Type");
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["PATH_INFO"] = uri;
    env["PATH_TRANSLATED"] = uri; // need to be change later
    env["QUERY_STRING"] = this->query;
    env["REMOTE_ADDR"] = "";  // need to be change later
    env["REMOTE_HOST"] = "";  // need to be change later
    env["REMOTE_IDENT"] = ""; // need to be change later
    env["REMOTE_USER"] = "";  // need to be change later
    env["REQUEST_METHOD"] = t_methodToString(request->headers.method);
    env["SCRIPT_NAME"] = location.CGI_path;
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
    std::cout << "initializing CGI" << std::endl;
    started = true;

    // CGItmpFile.getPath();
    tmpOutFilePath = CGItmpFile.getPath();

    // if (tmpout == NULL)
    // {
    //     std::cerr << "tmpfile error" << std::endl;
    //     handleError(500);
    //     return;
    // }
    // fdOut = fileno(tmpout);
    tmpOutFileName = generateRandomFileName("cgi_", ".out");
    std::cout << "tmpOutFileName: " << tmpOutFileName << std::endl;
    fdOut = open(("./tmp/" + tmpOutFileName).c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(fdOut == -1)
    {
        std::cerr << "open error" << std::endl;
        // handleError(500);
        exit(1);
        return;
    }
    // std::cout << "fdOut: " << fdOut << std::endl;
    bodyFd = open(("./tmp/" + uuid->getStr()).c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (bodyFd == -1)
    {
        std::cerr << "open error" << std::endl;
        handleError(500);
        return;
    }
    isCGIStarted = true;
    cgiStartTimer = time(NULL);
}

void Response::executeCGI()
{
    std::cout << "executing CGI" << std::endl;
    // exit(1);
    cgiPid = fork();
    if (cgiPid < 0)
    {
        close(fdOut);
        close(bodyFd);
        std::cerr << "fork error" << std::endl;
        handleError(500);
        return;
    }
    else if (cgiPid == 0)
    {
        // fdOut = open(tmpOutFilePath.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
        // if(fdOut == -1)
        // {
        //     std::cerr << "open error" << std::endl;
        //     handleError(500);
        //     return;
        // }
        // std::cout << "fdOut: " << fdOut << std::endl;
        bodyFd = open(("./tmp/" + uuid->getStr()).c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
        if (bodyFd == -1)
        {
            std::cerr << "open error" << std::endl;
            handleError(500);
            return;
        }

        std::cout << "child process" << std::endl;
        // CGItmpFile.freopen("r+", stdout);
        if (dup2(bodyFd, STDIN_FILENO) < 0 || dup2(fdOut, STDOUT_FILENO) < 0)
        {
            std::cerr << "dup2 error" << std::endl;
            exit(1);
        }
        // close(fdOut);
        // close(bodyFd);
        char **envp = getEnvironmentVariables();
        const char *args[] = {location.CGI_path.c_str(), locationPath.c_str(), NULL};
        // std::cout << "execve: " << location.CGI_path << std::endl;
        // std::cout << "execve: " << locationPath << std::endl;
        execve(location.CGI_path.c_str(), (char *const *)args, envp);
        std::cerr << "execve error" << std::endl;
        // std::cerr << "execve error" << std::endl;
        // std::cerr << "execve error: " << strerror(errno) << std::endl;
        exit(1);
    }
    else
    {
        //std::cout << "parent process" << std::endl;
        printf("parent process\n  ");
        close(bodyFd);
        close(fdOut);
    }
}

// check just  if child process is still running 
bool Response::checkGGIProcess()
{
   // std::cout << "checking CGI process" << std::endl;
    printf("checking CGI process\n");

    if (waitpid(cgiPid, &cgiStatus, WNOHANG) == 0)
        return true;
    else
        return false;
}

void Response::handleCGI()
{
    if(isCGIStarted)
    {
        if(checkGGIProcess())
        {
            handleCGIResponse();
        }
        // else
        // {
        //     //check if time is up
        //     if(time(NULL) - cgiStartTimer > 5)
        //     {
        //         std::cerr << "CGI timeout" << std::endl;
        //         handleError(500);
        //         return;
        //     }
        //     isCGIEnded = true;
        // }
    }
    else
    {
        initCGI();
        executeCGI();
    }
}

// when child process is done
// read from tmpfile
// generate header
// send response

// read from tmpfile till end of header
// generate header using header class
// send header
// read from tmpfile till end of file
// send body

std::string Response::extractCGIHeaders()
{
    std::cout << "extracting CGI headers" << std::endl;
    std::string header;
    char buffer[1024];
    int n;
    unsigned int i = 0;
    std::string tmpKey;
    std::string tmpValue;
    std::string tmp2 = "449595c7-4386-41f0-ac36-890931a89b97";

    CGItmpFile.open();


    // std::string tmp = CGItmpFile.getPath();
    // std::cout << "FD: " << fdOut << "\n";
    // fdOut = open("./tmp/output", O_RDONLY);
    
    // if(fdOut == -1)
    // {
    //     std::cerr << "open output error" << std::endl;
    //     exit(1);
    //     return "";
    // }


    // struct stat statbuf;
    // stat(tmp.c_str(), &statbuf);
    // std::cout << "File Name: " << tmp << "\n";
    // std::cout << "-------- statbuf.st_size: " << statbuf.st_size << std::endl;

    
    // std::cout << "-------- fdOut: " << fdOut << std::endl;
    // if(fdOut == -1)
    // {
    //     std::cerr << "open error" << std::endl;
    //     exit(1);
    //     return "";
    // }
    // n = read(fdOut, buffer, 1024);
    n = static_cast<int>(CGItmpFile.read(buffer, 1024));

    std::cout << "----------- n: " << n << std::endl;
    while (n > 0)
    {
        // printf("read %d bytes\n", n);
        printf("buffer: %s\n", buffer);
        header.append(buffer, n);
        if (header.find("\r\n\r\n") != std::string::npos)
            break;
        n = static_cast<int>(CGItmpFile.read(buffer, 1024));
    }
    CGItmpFile.close();
    while (i < header.size())
    {
        while (header[i] != ':')
        {
            tmpKey += header[i];
            i++;
        }
        i += 2;
        while (header[i] != '\r')
        {
            tmpValue += header[i];
            i++;
        }
        i += 2;
        this->header.setHeader(tmpKey, tmpValue);
        tmpKey.clear();
        tmpValue.clear();
    }
    // CGItmpFile.close();
    header.clear();
    header = this->header.getCGIHeader();
    std::cout << "Header: " << header << std::endl;
    return header;
}

void Response::handleCGIResponse()
{
    // if(ferror(tmpout))
    // {
    //     std::cerr << "error reading from tmpfile" << std::endl;
    //     exit(1);
    //    // handleError(500);
    //     return;
    // }

    
    std::cout << "handling CGI response" << std::endl;
    // std::string tmpBuffer;
    int size;
    if (!isHeaderSent)
    {
        //  fdOut = open("./tmp/449595c7-4386-41f0-ac36-890931a89b97", O_RDONLY);
        // if(fdOut == -1)
        // {
        //     std::cerr << "open output error" << std::endl;
        //     exit(1);
        // }
         //just for testing
        // cout << "extracting CGI headers" << endl;
        // bufferToSend = extractCGIHeaders();
        // make header
        fdOut = open(("./tmp/"+tmpOutFileName).c_str(), O_RDONLY);
        std::cout << "FD: " << fdOut << "\n";
        printf("File Name: %s\n", ("./tmp/"+tmpOutFileName).c_str());
        lseek(fdOut, 0, SEEK_SET);
        struct  stat statbuf;
        stat(("./tmp/"+ tmpOutFileName).c_str(), &statbuf);
        std::cout << "statbuf.st_size: " << statbuf.st_size << std::endl;
        header.setContentLength(statbuf.st_size);
        header.setHeader("Content-Type", "text/html");
        header.setHeader("Connection", "close");
        bufferToSend = header.getCGIHeader();
        isHeaderSent = true;

    }
    if (!reachedEOF)
    {   
        if(access(("./tmp/"+tmpOutFileName).c_str(), R_OK) != -1 )
        {
            std::cout << "File is open for reading." << std::endl;
        }
        cout << "reading from tmpfile" << endl;
        size = bufferToSend.size();
        char buffer[1024];
        int n;
        std::cout << "FD: " << fdOut << "\n";

        n = read(fdOut, buffer, 1024 - size);
        std::cout << "Reading from tmpfile: " << n << std::endl;
        if(errno)
        {
            cout << "here" << endl;
            perror("read");
            exit(1);
        }
        if (n > 0)
        {
            bufferToSend.append(buffer, n);
        }
        else
        {
            reachedEOF = true;
        }
    }
    cout << "sending response" << endl;
    sendNextChunk();
    if (bufferToSend.size() == 0)
    {
        ended = true;
    }
}