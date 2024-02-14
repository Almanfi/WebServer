#include "socket.hpp"

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


ErrorResponseStrategy::ErrorResponseStrategy(
    const string& error_code, IClientConf& config) :
    error_code(error_code), config(config),
    bufferToSend(), sentSize(0) {
}

transferState ErrorResponseStrategy::sendResponse(ISBuffer& buffer) {
    if (bufferToSend.empty()) {
        generateHeaders(); // TODO need content length
        generateBody();
    }
    if (sentSize < bufferToSend.size()) {
        size_t writeSize = buffer.write(bufferToSend.c_str() + sentSize,
                                        bufferToSend.size() - sentSize);
        sentSize += writeSize;
    }
    if (sentSize == bufferToSend.size())
        return COMPLETE;
    return INCOMPLETE;
}

void ErrorResponseStrategy::generateHeaders() {
    bufferToSend = "HTTP/1.1 " + error_code + " " + errorMsg() + "\r\n";
    bufferToSend+= "server: webserv\r\n";

    std::time_t now = std::time(NULL);
    std::tm* timeStruct = std::localtime(&now);
    char dateBuffer[100];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%a, %d %b %Y %H:%M:%S %Z", timeStruct);
    bufferToSend+= "date: " + std::string(dateBuffer) + "\r\n";
    
    bufferToSend+= "Connection: close\r\n";
    bufferToSend+= "Content-Type: text/html\r\n";
    bufferToSend+= "Content-Length: " + std::to_string(bufferToSend.size()) + "\r\n";
    bufferToSend+= "\r\n";
}

void ErrorResponseStrategy::generateErrorPage() {
    bufferToSend+= "<!DOCTYPE html>\n";
    bufferToSend+= "<html>\n";
    bufferToSend+= "<head>\n";
    bufferToSend+= "  <title>Error " + error_code + "</title>\n";
    bufferToSend+= "  <style>\n";
    bufferToSend+= "    * {\n";
    bufferToSend+= "      transition: all 0.6s;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    html {\n";
    bufferToSend+= "      height: 100%;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    body {\n";
    bufferToSend+= "      font-family: 'Lato', sans-serif;\n";
    bufferToSend+= "      color: #333;\n";
    bufferToSend+= "      margin: 0;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    #main {\n";
    bufferToSend+= "      display: table;\n";
    bufferToSend+= "      width: 100%;\n";
    bufferToSend+= "      height: 100vh;\n";
    bufferToSend+= "      text-align: center;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    .fof {\n";
    bufferToSend+= "      display: table-cell;\n";
    bufferToSend+= "      vertical-align: middle;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    .fof h1 {\n";
    bufferToSend+= "      font-size: 50px;\n";
    bufferToSend+= "      display: inline-block;\n";
    bufferToSend+= "      padding-right: 12px;\n";
    bufferToSend+= "      animation: type .5s alternate infinite;\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "    @keyframes type {\n";
    bufferToSend+= "      from { box-shadow: inset -3px 0px 0px #888; }\n";
    bufferToSend+= "      to { box-shadow: inset -3px 0px 0px transparent; }\n";
    bufferToSend+= "    }\n";
    bufferToSend+= "  </style>\n";
    bufferToSend+= "</head>\n";
    bufferToSend+= "<body>\n";
    bufferToSend+= "  <div id=\"main\">\n";
    bufferToSend+= "    <div class=\"fof\">\n";
    bufferToSend+= "      <h1>Error " + error_code + "</h1>\n";
    bufferToSend+= "      <p>" + errorMsg() + "</p>\n";
    bufferToSend+= "    </div>\n";
    bufferToSend+= "  </div>\n";
    bufferToSend+= "</body>\n";
    bufferToSend+= "</html>\n";
}

string ErrorResponseStrategy::errorMsg() { // TODO change this later
    switch (std::stoi(error_code))
    {
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    case 502:
        return "Bad Gateway";
    case 503:
        return "Service Unavailable";
    case 504:
        return "Gateway Timeout";
    default:
        return "Unknown Error";
    }
}

void ErrorResponseStrategy::generateBody() {
    string error_page = config.getErrorPage(error_code);
    if (error_page.empty() == true) {
        generateErrorPage();
        return;
    }
    fstream file(error_page, std::ios::in);
    if (file.is_open() == false) {
        generateErrorPage();
        return;
    }
    bufferToSend.append((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    file.close();
}

ResponseA::ResponseA(ISBuffer& buffer, IUniqFile& file,
            IHeader& headers, IClientConf& config) :
            started(false),ended(false),reachedEOF(false),
            buffer(buffer), requestBody(file),
            RequestHeader(headers), config(config),
            strategy(NULL) {
}

GetResponseStrategy::GetResponseStrategy(const string& path,
    const string& uri, IClientConf& config) :
    path(path), uri(uri), config(config), strategy(NULL) {
}

GetResponseStrategy::~GetResponseStrategy() {
    delete strategy;
}

void GetResponseStrategy::setDirectoryStrategy() {
    if (path.back() != '/') {
        strategy = new RedirectionResponseStrategy(path + "/", 301);
        return;
    }
    if (config.getInfo("autoindex") == "on") {
        DIR* dir = opendir(path.c_str());
        if (!dir) {
            if(errno == EACCES)
                strategy = new ErrorResponseStrategy("403", config);
            else
                strategy = new ErrorResponseStrategy("500", config);
            return;
        }
        strategy = new GetDirectoryResponseStrategy(path, uri, config, dir);
        if (!strategy)
            closedir(dir);
        return;
    }
    string locationIndexes = config.getInfo("index");
    size_t pos = 0;
    while (true) {
        size_t currentPos = pos;
        pos = locationIndexes.find(' ', pos);
        string index = locationIndexes.substr(currentPos, pos);
        string indexPath = path + index;
        if (pos == string::npos)
            break;
        if (access(indexPath.c_str(), F_OK) == 0) {
            strategy = new GetFileResponseStrategy(indexPath);
            return;
        }
    }
    strategy = new ErrorResponseStrategy("403", config);
}

void GetResponseStrategy::setStrategy() {
    struct stat statBuffer;
    if (stat(path.c_str(), &statBuffer) != 0) {
        switch (errno)
        {
        case ENOENT:
            strategy = new ErrorResponseStrategy("404", config);
            return;
        case EACCES:
            strategy = new ErrorResponseStrategy("403", config);
            return;
        default:
            strategy = new ErrorResponseStrategy("500", config);
            return;
        }
    }
    if (S_ISDIR(statBuffer.st_mode)) {
        setDirectoryStrategy();
        return;
    }
    if (S_ISREG(statBuffer.st_mode)) {
        strategy = new GetFileResponseStrategy(path);
        return;
    }
    strategy = new ErrorResponseStrategy("500", config);
}

transferState GetResponseStrategy::sendResponse(ISBuffer& buffer) {
    if (!strategy)
        setStrategy();
    return strategy->sendResponse(buffer);
}

GetFileResponseStrategy::GetFileResponseStrategy(const std::string& path) :
    path(path), sentSize(0) {
}

GetFileResponseStrategy::~GetFileResponseStrategy() {
}

transferState GetFileResponseStrategy::sendResponse(ISBuffer& buffer) {
    cout << "path: " << path << endl;
    cout << "sentSize: " << sentSize << endl;
    return (COMPLETE);
}

GetDirectoryResponseStrategy::GetDirectoryResponseStrategy(
    const std::string& path, const string& uri,
    const IClientConf& config, DIR* dir) :
    path(path), uri(uri), config(config),
    dir(dir), sentSize(0), bufferToSend() {
}

GetDirectoryResponseStrategy::~GetDirectoryResponseStrategy() {
    closedir(dir);
}

string GetDirectoryResponseStrategy::generateDirectoryListingPage() const {
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

string GetDirectoryResponseStrategy::generateListHTML(struct dirent* entry) const {
    std::stringstream html;
    struct stat buffer;
    string entityName = path + entry->d_name;
    if(stat((path + entry->d_name).c_str(), &buffer) != 0) // TODO why this?
        return "";
    if(entry->d_type == DT_DIR && std::strcmp(entry->d_name, "..") != 0) {

        html << "<tr><td><a href=\"" << uri + entry->d_name << "\">&#128193;" << " " << "<b>" << entry->d_name<< "/</b></a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td> - </td></tr>\n";
    }
    else if(std::strcmp(entry->d_name, "..") == 0) {
        if (uri == "/") // TODO we need this ???
            return;
        string parentDir = uri.substr(0, uri.find_last_of('/'));
        html << "<tr><td><a href=\"" << parentDir << "\">&#128281;" << " "<< entry->d_name << "/</a></td><td> </td><td>  </td></tr>\n";
    }
    else
        html << "<tr><td><a href=\"" << uri + entry->d_name << "\">&#128195;" << " "<< entry->d_name << "</a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td>" << buffer.st_size << "</td></tr>\n";
    return html.str();
}

transferState GetDirectoryResponseStrategy::sendResponse(ISBuffer& buffer) {
    if (bufferToSend.empty()) {
        std::string listingPageHTML;
        listingPageHTML = generateDirectoryListingPage();
        bufferToSend    = generateHeaders(listingPageHTML) + listingPageHTML;
    }
    if (sentSize < bufferToSend.size()) {
        size_t writeSize = buffer.write(bufferToSend.c_str() + sentSize, bufferToSend.size() - sentSize);
        sentSize += writeSize;
    }
    if (sentSize == bufferToSend.size())
        return COMPLETE;
    return INCOMPLETE;
}

string GetDirectoryResponseStrategy::generateHeaders(const string& body) const {
    std::stringstream headers;

    headers << "HTTP/1.1 200 OK\r\n";
    headers << "server: webserv\r\n";

    std::time_t now = std::time(NULL);
    std::tm* timeStruct = std::localtime(&now);
    char dateBuffer[100];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%a, %d %b %Y %H:%M:%S %Z", timeStruct);
    headers << "date: " << dateBuffer << "\r\n";

    headers << "Connection: close\r\n";
    headers << "Content-Type: text/html\r\n";
    headers << "Content-Length: " << body.size() << "\r\n";
    headers << "\r\n";
    return headers.str();
}

ResponseA::~ResponseA() {
    delete strategy;
}

transferState ResponseA::sendResponse(ISBuffer& buffer)
{
    if (!strategy)
        setStrategy();
    return strategy->sendResponse(buffer);
}

void ResponseA::setStrategy()
{
    t_method method = RequestHeader.getMethod();
    string& methodStr = Header::getMethodString(method);
    if (config.isAllowedMethod(methodStr) == false) {
        strategy = new ErrorResponseStrategy("405", config);
        return;
    }
    string return_value = config.getInfo("return");
    if (return_value != "") {
        size_t pos = return_value.find(" ");
        int code;
        stringstream ss(return_value.substr(0, pos));
        ss >> code;
        string location = return_value.substr(pos + 1);
        strategy = new RedirectionResponseStrategy(location, code);
        return;
    }
    switch (method)
    {
    case GET:
        strategy = new GetResponseStrategy(createPath(),
                        RequestHeader.getUri(), config);
        break;
    case POST:
        // strategy = new PostResponseStrategy(this, config.getRootPath());
        break;
    case DELETE:
        // strategy = new DeleteResponseStrategy(this, config.getRootPath());
        break;
    default:
        break;
    }
    // if (strategy == NULL)// TODO why this?
}

std::string ResponseA::createPath()
{
    string path1 = config.getInfo("root");
    string path2 = RequestHeader.getUri();
    // if (path2.back() == '/' && path2.size() > 1)
    //     path2.pop_back(); // TODO if dir redirect to path+'/'
    if(path1[path1.size() - 1] == '/' && path2[0] == '/')
        return path1 + path2.substr(1);
    else if(path1[path1.size() - 1] != '/' && path2[0] != '/')
        return path1 + "/" + path2;
    else
        return path1 + path2;
}


Response::Response():started(false),ended(false),reachedEOF(false)
{
}

Response::~Response()
{
}

void Response::initResponse(Client *client)
{
    // this->request = &client->request;
    // this->servSock = &client->servSock;
    // this->uuid = &client->uuid;
    // this->fd = client->fd;
    // this->method = request->headers.method;
    // this->uri = request->headers.uri;
    // setLocation();
    this->locationPath = joinPath(location.root , uri);
    //  need to check if the file exists
    if(stat((location.root+uri).c_str(),&buff) != 0)
    {
        if(errno == ENOENT)
            handleError(404);
        else if(errno == EACCES)
            handleError(403);
        else
            handleError(500);
        return;
    }
    cout << "++++++++++++ initResponse ++++++++++++" << endl;

}


// void Response::setLocation()
// {
//     this->location.methods.push_back(GET);
//     this->location.methods.push_back(POST);
//     this->location.root = "./nginx-html";
//     this->location.index.push_back("index.html");
//     this->location.index.push_back("index.htm");
//     this->location.index.push_back("page8.html");
//     this->location.error_page[404] = "404.html";
//     this->location.error_page[500] = "500.html";
//     this->location.autoindex = true;
//     this->location.return_code = 0;
//     this->location.return_url = "";
// }

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
    if(S_ISDIR(buff.st_mode))
        handleDirectory();
    else if(S_ISREG(buff.st_mode))
        handleFile();
    else
        handleError(404);
}

void Response::handlePost()
{
    

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

// bool Response::isEnded()
// {
//     return ended;
// }

// bool Response::isStarted()
// {
//     return started;
// }
// void Response::sendDirectory(const std::string &path)
// {
//     DIR *dir= NULL;
//     std::string listingPageHTML;
    
//     if(!started)
//     {
//         dir = opendir(path.c_str());
//         if (!dir) {
//             // perror("opendir");
//             if(errno == EACCES)
//                 return handleError(403);
//             else
//                 return handleError(500);
//         }
//         listingPageHTML = generateDirectoryListingPage(dir);
//         header.setStatusCode(301);
//         header.setHeader("Connection", "close");
//         header.setHeader("Content-Type", "text/html");
//         header.setContentLength(listingPageHTML.size());
//         bufferToSend = header.getHeader();

//         started = true;
//     }
    
//     bufferToSend.append(listingPageHTML);
//     sendNextChunk();
//     if(bufferToSend.size() == 0)
//         ended = true; 
//     closedir(dir);
// }

// std::string Response::generateErrorPage(int errorCode, const std::string& errorMsg) {
//     std::stringstream htmlPage;

//     htmlPage << "<!DOCTYPE html>\n";
//     htmlPage << "<html>\n";
//     htmlPage << "<head>\n";
//     htmlPage << "  <title>Error " << errorCode << "</title>\n";
//     htmlPage << "  <style>\n";
//     htmlPage << "    * {\n";
//     htmlPage << "      transition: all 0.6s;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    html {\n";
//     htmlPage << "      height: 100%;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    body {\n";
//     htmlPage << "      font-family: 'Lato', sans-serif;\n";
//     htmlPage << "      color: #333;\n";
//     htmlPage << "      margin: 0;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    #main {\n";
//     htmlPage << "      display: table;\n";
//     htmlPage << "      width: 100%;\n";
//     htmlPage << "      height: 100vh;\n";
//     htmlPage << "      text-align: center;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    .fof {\n";
//     htmlPage << "      display: table-cell;\n";
//     htmlPage << "      vertical-align: middle;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    .fof h1 {\n";
//     htmlPage << "      font-size: 50px;\n";
//     htmlPage << "      display: inline-block;\n";
//     htmlPage << "      padding-right: 12px;\n";
//     htmlPage << "      animation: type .5s alternate infinite;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    @keyframes type {\n";
//     htmlPage << "      from { box-shadow: inset -3px 0px 0px #888; }\n";
//     htmlPage << "      to { box-shadow: inset -3px 0px 0px transparent; }\n";
//     htmlPage << "    }\n";
//     htmlPage << "  </style>\n";
//     htmlPage << "</head>\n";
//     htmlPage << "<body>\n";
//     htmlPage << "  <div id=\"main\">\n";
//     htmlPage << "    <div class=\"fof\">\n";
//     htmlPage << "      <h1>Error " << errorCode << "</h1>\n";
//     htmlPage << "      <p>" << errorMsg << "</p>\n";
//     htmlPage << "    </div>\n";
//     htmlPage << "  </div>\n";
//     htmlPage << "</body>\n";
//     htmlPage << "</html>\n";

//     return htmlPage.str();
// }

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

// std::string Response::generateListHTML(struct dirent* entry)
// {
//     std::stringstream html;
//     struct stat buffer;
//     if(stat(joinPath(locationPath, entry->d_name).c_str(), &buffer) != 0)
//         return "";
//     if(entry->d_type == DT_DIR && std::strcmp(entry->d_name, "..") != 0)
//         html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128193;" << " " << "<b>" << entry->d_name<< "/</b></a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td> - </td></tr>\n";
//     else if(std::strcmp(entry->d_name, "..") == 0)
//         html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128281;" << " "<< entry->d_name << "/</a></td><td> </td><td>  </td></tr>\n";
//     else
//         html << "<tr><td><a href=\"" << joinPath(uri,entry->d_name) << "\">&#128195;" << " "<< entry->d_name << "</a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td>" << buffer.st_size << "</td></tr>\n";
//     return html.str();
// }

// std::string Response::generateDirectoryListingPage(DIR* dir) 
// {
//     std::stringstream htmlPage;
//     struct dirent* entry;
//     std::vector<struct dirent*> entries;
//     while((entry = readdir(dir)) != NULL)
//     {
//         entries.push_back(entry);
//     }

//     htmlPage << "<!DOCTYPE html>\n";
//     htmlPage << "<html>\n";
//     htmlPage << "<head>\n";
//     htmlPage << "  <title>Directory Listing</title>\n";
//     htmlPage << "  <style>\n";
//     htmlPage << "    * {\n";
//     htmlPage << "      background: #333;\n";
//     htmlPage << "      transition: all 0.6s;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    html {\n";
//     htmlPage << "      height: 100%;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    body {\n";
//     htmlPage << "      font-family: 'Lato', sans-serif;\n";
//     htmlPage << "      margin: 0;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    #main {\n";
//     htmlPage << "      width: 100%;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    table {\n";
//     htmlPage << "      width: 80%;\n";
//     htmlPage << "      margin: 0 auto;\n";
//     htmlPage << "      border-collapse: collapse;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    th, td {\n";
//     htmlPage << "      border: 1px solid #333;\n";
//     htmlPage << "      padding: 6px;\n";
//     htmlPage << "      text-align: left;\n";
//     htmlPage << "      color: #fff;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    a {\n";
//     htmlPage << "      color: #fff;\n";
//     htmlPage << "      text-decoration: none;\n";
//     htmlPage << "    }\n";
//     htmlPage << "    h1 {\n";
//     htmlPage << "      color: #fff;\n";
//     htmlPage << "      margin-left: 10%;\n";
//     htmlPage << "      margin-top: 2%;\n";
//     htmlPage << "      margin-bottom: 2%;\n";
//     htmlPage << "    }\n";
//     htmlPage << "  </style>\n";
//     htmlPage << "</head>\n";
//     htmlPage << "<body>\n";
//     htmlPage << "  <div id=\"main\">\n";
//     htmlPage << "    <h1> Index of " << uri << "</h1>\n";
//     htmlPage << "    <table>\n";
//     std::sort(entries.begin(), entries.end(), compareDirent);
//     for(size_t i = 0; i < entries.size(); i++)
//     {
//         if(entries[i]->d_name[0] == '.' && std::strcmp(entries[i]->d_name, "..") != 0)
//             continue;
//         htmlPage << generateListHTML(entries[i]);
//     }
//     htmlPage << "    </table>\n";
//     htmlPage << "  </div>\n";
//     htmlPage << "</body>\n";
//     htmlPage << "</html>\n";

//     return htmlPage.str();
// }

// bool Response::checkForValidMethod()
// {
//     for(size_t i = 0; i < location.methods.size(); i++)
//     {
//         if(location.methods[i] == request->headers.method)
//             return true;
//     }
//     return false;
// }

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

void Response::handleDelete()
{

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