
#include "../../include/response/utils.hpp" 


bool compareDirent(const struct dirent *a, const struct dirent *b)
{
    if (a->d_type == DT_DIR && b->d_type != DT_DIR)
        return true;
    return std::strcmp(a->d_name, b->d_name) < 0;
}

std::string formatDateTime(time_t timestamp)
{
    struct tm *localTime = localtime(&timestamp);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return buffer;
}

std::string getExtension(const std::string &path)
{
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos)
        return "";
    return path.substr(pos + 1);
}

std::string toLower(const std::string &str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool isImage(const std::string &ext)
{
    return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "apng" || ext == "avif" || ext == "jfif" || ext == "pjpeg" || ext == "pjp" || ext == "svg" || ext == "webp";
}

bool isVideo(const std::string &ext)
{
    return ext == "avi" || ext == "flv" || ext == "mov" || ext == "mp4" || ext == "mpg" || ext == "mpeg" || ext == "wmv";
}

bool isAudio(const std::string &ext)
{
    return ext == "mp3" || ext == "wav" || ext == "wma" || ext == "aac" || ext == "flac" || ext == "ogg" || ext == "alac" || ext == "aiff";
}

std::string formatSize(size_t size)
{
    std::stringstream formattedSize;
    if (size < 1024)
        formattedSize << size << " B";
    else if (size < 1024 * 1024)
        formattedSize << size / 1024 << " KB";
    else if (size < 1024 * 1024 * 1024)
        formattedSize << size / (1024 * 1024) << " MB";
    else
        formattedSize << size / (1024 * 1024 * 1024) << " GB";
    return formattedSize.str();
}

std::string generateErrorPage(int errorCode, const std::string &errorMsg)
{
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

std::string generateRandomFileName(const std::string &prefix, const std::string &extension)
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    int randomValue = std::rand();
    std::stringstream ss;
    ss << randomValue;
    std::string fileName = prefix + ss.str() + extension;
    return fileName;
}

std::string joinPath(const std::string &path1, const std::string &path2)
{
    if (path1[path1.size() - 1] == '/' && path2[0] == '/')
        return path1 + path2.substr(1);
    else if (path1[path1.size() - 1] != '/' && path2[0] != '/')
        return path1 + "/" + path2;
    else
        return path1 + path2;
}

std::string decodingURI(const std::string &uri)
{
    std::string decodedURI;
    char *end;
    size_t i = 0;
    while (i < uri.size())
    {
        if (uri[i] == '%')
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
    return (decodedURI);
}

std::string generateListHTML(struct dirent *entry, std::string uri, std::string locationPath)
{
    std::stringstream html;
    struct stat buffer;
    if (stat(joinPath(locationPath, entry->d_name).c_str(), &buffer) != 0)
        return "";
    if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, "..") != 0)
        html << "<tr><td><a href=\"" << joinPath(uri, entry->d_name) << "\">&#128193;"
             << " "
             << "<b>" << entry->d_name << "/</b></a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td> - </td></tr>\n";
    else if (std::strcmp(entry->d_name, "..") == 0)
        html << "<tr><td><a href=\"" << joinPath(uri, entry->d_name) << "\">&#128281;"
             << " " << entry->d_name << "/</a></td><td> </td><td>  </td></tr>\n";
    else
    {
        std::string icone;
        std::string ext = toLower(getExtension(entry->d_name));
        if (isImage(ext))
        {
            std::cout << "isImage" << std::endl;
            icone = "&#128247;";
        }
        else if (isVideo(ext))
            icone = "&#128253;";
        else if (isAudio(ext))
            icone = "&#127925;";
        else
            icone = "&#128195;";

        html << "<tr><td><a href=\"" << joinPath(uri, entry->d_name) << "\">" << icone
             << " " << entry->d_name << "</a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td>" << formatSize(buffer.st_size) << "</td></tr>\n";
    }
    return html.str();
}
std::string generateDirectoryListingPage(DIR *dir , std::string uri, std::string locationPath)
{
    std::stringstream htmlPage;
    struct dirent *entry;
    std::vector<struct dirent *> entries;
    while ((entry = readdir(dir)) != NULL)
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
    for (size_t i = 0; i < entries.size(); i++)
    {
        if (entries[i]->d_name[0] == '.' && std::strcmp(entries[i]->d_name, "..") != 0)
            continue;
        htmlPage << generateListHTML(entries[i], uri, locationPath);
    }
    htmlPage << "    </table>\n";
    htmlPage << "  </div>\n";
    htmlPage << "</body>\n";
    htmlPage << "</html>\n";

    return htmlPage.str();
}

std::string convertT_method(t_method method)
{
    if (method == GET)
        return "GET";
    else if (method == POST)
        return "POST";
    else if (method == DELETE)
        return "DELETE";
    return "INVALID";
}

std::string ToString(int num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string FormatEnvKey(const std::string &key)
{
    std::string formattedKey = key;
    std::transform(formattedKey.begin(), formattedKey.end(), formattedKey.begin(), ::toupper);
    std::replace(formattedKey.begin(), formattedKey.end(), '-', '_');
    return formattedKey;
}