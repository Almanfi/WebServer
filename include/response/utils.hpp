#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <ctime>
#include "socket.hpp"


std::string generateRandomFileName(const std::string &prefix, const std::string &extension);
std::string generateErrorPage(int errorCode, const std::string &errorMsg);
std::string formatDateTime(time_t timestamp);
std::string getExtension(const std::string &path);
std::string decodingURI(const std::string &uri);
std::string formatSize(size_t size);
std::string joinPath(const std::string &path1, const std::string &path2);
std::string toLower(const std::string &str);
std::string convertT_method(t_method method);
std::string ToString(int num);
std::string FormatEnvKey(const std::string &key);
bool compareDirent(const struct dirent *a, const struct dirent *b);
bool isImage(const std::string &ext);
bool isVideo(const std::string &ext);
bool isAudio(const std::string &ext);
std::string generateListHTML(struct dirent *entry, std::string uri, std::string locationPath);
std::string generateDirectoryListingPage(DIR *dir , std::string uri , std::string locationPath);
#endif