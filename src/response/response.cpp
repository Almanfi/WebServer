#include "socket.hpp"

Response::Response():started(false),ended(false),reachedEOF(false)
{
}

Response::~Response()
{
}

void Response::initResponse(Client *client)
{
    this->request = &client->request;
    this->servSock = &client->servSock;
    this->uuid = &client->uuid;
    this->fd = client->fd;
    this->method = request->headers.method;
    this->uri = decodingURI(request->headers.uri);
    std::cout << "uri: " << uri << std::endl;
    this->bodyPath = request->body;
    setLocation();
    this->locationPath = joinPath(location.root , uri);
    cout << "++++++++++++ initResponse ++++++++++++" << endl;

}


void Response::setLocation()
{
    
    this->location.methods.push_back(GET);
    this->location.methods.push_back(POST);
    this->location.methods.push_back(DELETE);
    this->location.root = "./nginx-html";
    this->location.index.push_back("index.html");   
    this->location.index.push_back("index.htm");
    this->location.index.push_back("page8.html");
    this->location.error_page[404] = "404.html";
    this->location.error_page[500] = "500.html";
    this->location.autoindex = true;
    this->location.return_code = 0;
    this->location.return_url = "";
    this->location.allow_upload = true;
    this->location.upload_path = "nginx-html/";
    this->location.allow_CGI = true;
    this->location.CGI_path = "user/bin/php-cgi";

}
void Response::sendResponse()
{
    cout << "++++++++++++ sendResponse ++++++++++++" << endl;
        if(!checkForValidMethod())
            handleError(405);
        if(handleRedirection())
            return;
        if(method == GET)
            handleGet();
        else if(method == POST)
            handlePost();
        else if(method == DELETE)
            handleDelete();
        //started = true;
}


void Response::handleGet()
{
    cout << "++++++++++++ handleGet ++++++++++++" << endl;
    if(stat(locationPath.c_str(), &buff) != 0)
    {
        if(errno == ENOENT)
            handleError(404);
        else if(errno == EACCES)
            handleError(403);
        else
            handleError(500);
        return;
    }
    if(S_ISDIR(buff.st_mode))
        handleDirectory();
    else if(S_ISREG(buff.st_mode))
        handleFile();
    else
        handleError(404);
}

void Response::handlePost()
{
    std::cout << "++++++++++++ handlePost ++++++++++++" << std::endl;
    std::string newPath = joinPath( location.upload_path,uri);
    std::string oldPath = joinPath("./tmp", uuid->getStr());
    if(location.allow_upload)
    {
        if(rename(oldPath.c_str(), newPath.c_str()) != 0)
        {
            if(errno == ENOENT)
                handleError(404);
            else if(errno == EACCES)
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
        if(bufferToSend.size() == 0)
            ended = true;
        return;
    }
    else
        handleError(403);

}
void Response::handleDelete()
{ 
    std::cout << "++++++++++++ handleDelete ++++++++++++" << std::endl;
    std::string pathToDelete = joinPath(location.upload_path, uri);
    if( location.allow_upload)
    {
        if(stat(pathToDelete.c_str(), &buff) != 0)
        {
            if(errno == ENOENT)
                handleError(404);
            else if(errno == EACCES)
                handleError(403);
            else
                handleError(500);
            return;
        }
        if(S_ISDIR(buff.st_mode))
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
    if(bufferToSend.size() == 0)
        ended = true;
    return;
}

void Response::handleDirectory()
{
    for(size_t i = 0 ; i < location.index.size(); i++)
    {
        std::string path = joinPath(locationPath, location.index[i]);
        if(stat(path.c_str(),&buff) == 0 && S_ISREG(buff.st_mode))
        {
            sendFile(path);
            return;
        }
    }
    if(location.autoindex)
        sendDirectory(location.root+uri);
    else
        handleError(403);
}
void Response::handleFile()
{
    sendFile(location.root+uri);
}

void Response::sendFile(const std::string &path)
{
    int size = 0;
    std::cout << "++++++++++++++++++++++ in sendFile ++++++++++++++++++++++++++++" << std::endl;
    if(!started)
    {
        header.setStatusCode(200);
        header.setConnection("close");
        header.setContentType(path);
        header.setContentLength(buff.st_size);
        bufferToSend = header.getHeader();
        fileToSend.open(path.c_str(), std::ios::in | std::ios::out | std::ios::app);
        if (!fileToSend.is_open()) {
            // perror("open");
            if(errno == EACCES)
                return handleError(403);
            else
                return handleError(500);
        }
        started = true;
    }
    if(!reachedEOF)
    {
        size = bufferToSend.size();
        char tmpBuffer[1024];
        fileToSend.read(tmpBuffer, 1024 - size);
        size_t readSize = fileToSend.gcount();
        if(fileToSend.eof())
        {
            reachedEOF = true;
            fileToSend.close();
        
        }
        bufferToSend.append(tmpBuffer, readSize);
    }
    sendNextChunk();
    if(bufferToSend.size() == 0 && reachedEOF)
        ended = true;
}


void Response::sendNextChunk()
{
    int sendedSize = 0;
    sendedSize = send(fd, bufferToSend.c_str(), bufferToSend.size(), 0);
    if(sendedSize == -1)
    {
        perror("send");
        ended = true;
    }
    bufferToSend.erase(0, sendedSize);
}

void Response::handleError(int error_code)
{
    std::string errorPage;
    if(location.error_page.find(error_code) != location.error_page.end())
    {
        std::string path = joinPath(locationPath, location.error_page[error_code]);
        if(stat(path.c_str(),&buff) == 0 && S_ISREG(buff.st_mode))
        {
            sendFile(path);
            return;
        }

    }
    errorPage = generateErrorPage(error_code , header.getStatusMessage(error_code));
    if(!started)
    {
        header.setStatusCode(error_code);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setContentLength(errorPage.size());
        bufferToSend = header.getHeader();
        started = true;
    }
    bufferToSend.append(errorPage);
    sendNextChunk(); 
    if(bufferToSend.size() == 0)
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
    DIR *dir;
    std::string listingPageHTML;
    
    if(!started)
    {
        dir = opendir(path.c_str());
        if (!dir) {
            // perror("opendir");
            if(errno == EACCES)
                return handleError(403);
            else
                return handleError(500);
        }
        listingPageHTML = generateDirectoryListingPage(dir);
        header.setStatusCode(301);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setContentLength(listingPageHTML.size());
        bufferToSend = header.getHeader();

        started = true;
    }
    
    bufferToSend.append(listingPageHTML);
    sendNextChunk();
    if(bufferToSend.size() == 0)
        ended = true; 
    closedir(dir);
}

std::string Response::generateErrorPage(int errorCode, const std::string& errorMsg) {
    std::stringstream htmlPage;

    htmlPage << "<!DOCTYPE html>\n";
    htmlPage << "<html>\n";
    htmlPage << "<head>\n";
    htmlPage << "  <title>Error " << errorCode << "</title>\n";
    htmlPage << "  <style>\n";
    htmlPage << "    * {\n";
    htmlPage << "      transition: all 0.6s;\n";
    htmlPage << "    }\n";
    htmlPage << "    html {\n";
    htmlPage << "      height: 100%;\n";
    htmlPage << "    }\n";
    htmlPage << "    body {\n";
    htmlPage << "      font-family: 'Lato', sans-serif;\n";
    htmlPage << "      color: #333;\n";
    htmlPage << "      margin: 0;\n";
    htmlPage << "    }\n";
    htmlPage << "    #main {\n";
    htmlPage << "      display: table;\n";
    htmlPage << "      width: 100%;\n";
    htmlPage << "      height: 100vh;\n";
    htmlPage << "      text-align: center;\n";
    htmlPage << "    }\n";
    htmlPage << "    .fof {\n";
    htmlPage << "      display: table-cell;\n";
    htmlPage << "      vertical-align: middle;\n";
    htmlPage << "    }\n";
    htmlPage << "    .fof h1 {\n";
    htmlPage << "      font-size: 50px;\n";
    htmlPage << "      display: inline-block;\n";
    htmlPage << "      padding-right: 12px;\n";
    htmlPage << "      animation: type .5s alternate infinite;\n";
    htmlPage << "    }\n";
    htmlPage << "    @keyframes type {\n";
    htmlPage << "      from { box-shadow: inset -3px 0px 0px #888; }\n";
    htmlPage << "      to { box-shadow: inset -3px 0px 0px transparent; }\n";
    htmlPage << "    }\n";
    htmlPage << "  </style>\n";
    htmlPage << "</head>\n";
    htmlPage << "<body>\n";
    htmlPage << "  <div id=\"main\">\n";
    htmlPage << "    <div class=\"fof\">\n";
    htmlPage << "      <h1>Error " << errorCode << "</h1>\n";
    htmlPage << "      <p>" << errorMsg << "</p>\n";
    htmlPage << "    </div>\n";
    htmlPage << "  </div>\n";
    htmlPage << "</body>\n";
    htmlPage << "</html>\n";

    return htmlPage.str();
}

static bool compareDirent(const struct dirent* a, const struct dirent* b)
{
    if(a->d_type == DT_DIR && b->d_type != DT_DIR)
        return true;
    return std::strcmp(a->d_name, b->d_name) < 0;
}
static std::string formatDateTime(time_t timestamp)
{
    struct tm* localTime = localtime(&timestamp);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return buffer;
}

std::string Response::generateListHTML(struct dirent* entry)
{
    std::stringstream html;
    struct stat buffer;
    if(stat(joinPath(locationPath, entry->d_name).c_str(), &buffer) != 0)
        return "";
    if(entry->d_type == DT_DIR && std::strcmp(entry->d_name, "..") != 0)
        html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128193;" << " " << "<b>" << entry->d_name<< "/</b></a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td> - </td></tr>\n";
    else if(std::strcmp(entry->d_name, "..") == 0)
        html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128281;" << " "<< entry->d_name << "/</a></td><td> </td><td>  </td></tr>\n";
    else
        html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128195;" << " "<< entry->d_name << "</a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td>" << buffer.st_size << "</td></tr>\n";
    return html.str();
}
std::string Response::generateDirectoryListingPage(DIR* dir) 
{
    std::stringstream htmlPage;
    struct dirent* entry;
    std::vector<struct dirent*> entries;
    while((entry = readdir(dir)) != NULL)
    {
        entries.push_back(entry);
    }

    htmlPage << "<!DOCTYPE html>\n";
    htmlPage << "<html>\n";
    htmlPage << "<head>\n";
    htmlPage << "  <title>Directory Listing</title>\n";
    htmlPage << "  <style>\n";
    htmlPage << "    * {\n";
    htmlPage << "      background: #333;\n";
    htmlPage << "      transition: all 0.6s;\n";
    htmlPage << "    }\n";
    htmlPage << "    html {\n";
    htmlPage << "      height: 100%;\n";
    htmlPage << "    }\n";
    htmlPage << "    body {\n";
    htmlPage << "      font-family: 'Lato', sans-serif;\n";
    htmlPage << "      margin: 0;\n";
    htmlPage << "    }\n";
    htmlPage << "    #main {\n";
    htmlPage << "      width: 100%;\n";
    htmlPage << "    }\n";
    htmlPage << "    table {\n";
    htmlPage << "      width: 80%;\n";
    htmlPage << "      margin: 0 auto;\n";
    htmlPage << "      border-collapse: collapse;\n";
    htmlPage << "    }\n";
    htmlPage << "    th, td {\n";
    htmlPage << "      border: 1px solid #333;\n";
    htmlPage << "      padding: 6px;\n";
    htmlPage << "      text-align: left;\n";
    htmlPage << "      color: #fff;\n";
    htmlPage << "    }\n";
    htmlPage << "    a {\n";
    htmlPage << "      color: #fff;\n";
    htmlPage << "      text-decoration: none;\n";
    htmlPage << "    }\n";
    htmlPage << "    h1 {\n";
    htmlPage << "      color: #fff;\n";
    htmlPage << "      margin-left: 10%;\n";
    htmlPage << "      margin-top: 2%;\n";
    htmlPage << "      margin-bottom: 2%;\n";
    htmlPage << "    }\n";
    htmlPage << "  </style>\n";
    htmlPage << "</head>\n";
    htmlPage << "<body>\n";
    htmlPage << "  <div id=\"main\">\n";
    htmlPage << "    <h1> Index of " << uri << "</h1>\n";
    htmlPage << "    <table>\n";
    std::sort(entries.begin(), entries.end(), compareDirent);
    for(size_t i = 0; i < entries.size(); i++)
    {
        if(entries[i]->d_name[0] == '.' && std::strcmp(entries[i]->d_name, "..") != 0)
            continue;
        htmlPage << generateListHTML(entries[i]);
    }
    htmlPage << "    </table>\n";
    htmlPage << "  </div>\n";
    htmlPage << "</body>\n";
    htmlPage << "</html>\n";

    return htmlPage.str();
}

bool Response::checkForValidMethod()
{
    for(size_t i = 0; i < location.methods.size(); i++)
    {
        if(location.methods[i] == request->headers.method)
            return true;
    }
    return false;
}

bool Response::handleRedirection()
{
    if(location.return_code >= 301 && location.return_code <= 399)
    {
        header.setStatusCode(location.return_code);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "text/html");
        header.setHeader("Location", location.return_url);
        bufferToSend = header.getHeader();
        sendNextChunk();
        if(bufferToSend.size() == 0)
            ended = true;
        return true;
    }
    else if (location.return_code >= 100 && location.return_code < 599 )
    {
        header.setStatusCode(location.return_code);
        header.setHeader("Connection", "close");
        header.setHeader("Content-Type", "application/octet-stream");
        header.setContentLength(location.return_url.size());
        bufferToSend = header.getHeader();
        bufferToSend.append(location.return_url);
        sendNextChunk();
        if(bufferToSend.size() == 0)
            ended = true;
        return true;
    }
    return false;
}

std::string Response::joinPath(const std::string &path1, const std::string &path2)
{
    if(path1[path1.size() - 1] == '/' && path2[0] == '/')
        return path1 + path2.substr(1);
    else if(path1[path1.size() - 1] != '/' && path2[0] != '/')
        return path1 + "/" + path2;
    else
        return path1 + path2;
}

std::string Response::decodingURI(const std::string &uri)
{
    std::string decodedURI;
    char *end;
    size_t i = 0;
    while(i < uri.size())
    {
        if(uri[i] == '%')
        {
            char c = std::strtol(uri.substr(i + 1, 2).c_str(), &end, 16);
            decodedURI.push_back(c);
            i += 3;
        }
        else
        {
            decodedURI.push_back(uri[i]);
            i++;
        }
    }
    return(decodedURI) ;
}

// void Response::uriParser()
// {
   
//     size_t pos = uri.find('?');
//     if(pos != std::string::npos)
//     {   std::string newURI;
//         newURI = uri.substr(0, pos);
//         std::string query = uri.substr(pos + 1);
//         std::stringstream ss(query);
//         std::string key;
//         std::string value;
//         while(std::getline(ss, key, '=') && std::getline(ss, value, '&'))
//             query[key] = value;
//         uri = newURI;
//     }
// }

