#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"
#include "../include/Location.hpp"
#include "../include/Debug.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>


/**
 * @brief Handles GET requests. Serves file, index, or autoindex.
 */
void Response::handleGetMethod(Request& request){
    struct stat sb;
   
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError();
        return;
    }
    if (!S_ISDIR(sb.st_mode)){
        serveFile();
        return ;
    }
    if (request.getPath()[request.getPath().size() - 1] != '/'){
        _statusCode = MOVED_PERMANENTLY;
        _headers["Location"] = request.getPath() + "/";
        return ;
    }
    std::vector<std::string> indexes = _loc->getIndexes();
    std::vector<std::string>::iterator it = indexes.begin();
    for (; it != indexes.end(); it++){
        _internalPath = *it;
        if (serveFile())
            return ;
    }
    if (!_loc->getAutoIndex()){
        _statusCode = FORBIDDEN;
        buildError();
        return ;
    }
    displayAutoIndex();
}

/**
 * @brief Opens and reads _internalPath into _body. Sets 200 on success.
 * @return true on success, false on failure (sets error status).
 */
bool Response::serveFile() {
    std::ifstream file(_internalPath.c_str());
    if (!file.is_open()) {
        setSyscallError();
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    _body = ss.str();
    _statusCode = OK;
    _headers["Content-Type"] = extractContentType();
    return true;
}


/**
 * @brief Generates an HTML directory listing for _internalPath.
 */
void Response::displayAutoIndex(){
    DIR *dir = opendir(_internalPath.c_str());
    if (dir == NULL){
        setSyscallError();
        return ;
    }
    std::string html = "<html><body><h1>Index of " + _internalPath + "</h1><ul>";
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL ){
        std::string name = entry->d_name;
        if (name == ".") continue;
        html += "<li><a href=\"" + name + "\">" + name + "</a></li>";
    }
    closedir(dir);
    html += "</ul></body></html>";
    _body = html;
    _statusCode = OK;
    _headers["Content-Type"] = "text/html";
}

/**
 * @brief Handles POST requests. Creates a new file in upload_store.
 */
void Response::handlePostMethod(Request& request){
    if (_remain.empty() || _remain[_remain.size() - 1] == '/') {
        _statusCode = BAD_REQUEST;
        buildError();
        LOG(std::cout << "can't upload body without specifying the file in URL" << std::endl);
        return;
    }
    if (request.getHeaders("content-type").find("multipart/form-data") != std::string::npos){
        _statusCode = FORBIDDEN;
        buildError();
        LOG(std::cout << "can't handle multipart without cgi" << std::endl);
        return ;
    }
    if (_loc->getUploadStore().empty()){
        _statusCode = FORBIDDEN;
        buildError();
        LOG(std::cout << "can't upload, no upload store" << std::endl);
        return ;
    }
    std::string uploadDir = _loc->getUploadStore();
    struct stat dirStat;
    if (stat(uploadDir.c_str(), &dirStat) != 0 || !S_ISDIR(dirStat.st_mode)){
        setSyscallError();
        LOG(std::cout << "upload folder doesnt exist" << std::endl);
        return ;
    }
    std::string uploadPath = _loc->getUploadStore() + "/" + _remain;
    struct stat fileStat;
    if (stat(uploadPath.c_str(), &fileStat) == 0){
        _statusCode = CONFLICT;
        buildError();
        LOG(std::cout << "file already exist" << std::endl);
        return ;
    }
    std::ofstream file(uploadPath.c_str());
    if (!file.is_open()) {
        setSyscallError();
        return;
    }
    file << request.getBody();
    file.close();
    _body = request.getBody();
    _statusCode = CREATED;
    _headers["Content-Type"] = request.getHeaders("content-type");
}

/**
 * @brief Handles DELETE requests. Removes the target file.
 */
void Response::handleDeleteMethod(Request& request){
    struct stat sb;
   
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError();
        return;
    }
    if (S_ISDIR(sb.st_mode)){
        if (request.getPath()[request.getPath().size() - 1] != '/'){
            _statusCode = CONFLICT;
            return ;
        }
    }
    if (std::remove(_internalPath.c_str()) != 0){
        setSyscallError();
        return;
    }
    _statusCode = NO_CONTENT;
}