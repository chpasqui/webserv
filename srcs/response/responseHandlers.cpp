#include "../../include/Response.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>

/**
 * @brief Handles GET requests. Serves file, index, or autoindex.
 */
void Response::handleGetMethod(Request& request){
    struct stat sb;

    if (request.getPath() == "/login.html" && !_session->data["user"].empty()){
        _statusCode = MOVED_PERMANENTLY;
        _headers["Location"] = "/";
        return ;
    }
    if (request.getPath() == "/logout" && !_session->data["user"].empty()){
        serverLogoutPage();
        return;
    }
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError("handleGetMethod: stat failed");
        return;
    }
    if (!S_ISDIR(sb.st_mode)){
        if (serveFile())
            return ;
        setSyscallError("serveFile: file can not open");
    }
    if (request.getPath()[request.getPath().size() - 1] != '/'){
        _statusCode = MOVED_PERMANENTLY;
        _headers["Location"] = request.getPath() + "/";
        return ;
    }
    std::string dirPath = _internalPath;
    std::vector<std::string> indexes = _loc->getIndexes();
    std::vector<std::string>::iterator it = indexes.begin();
    for (; it != indexes.end(); it++){
        _internalPath = dirPath + *it;
        if (serveFile())
            return ;
    }
    _internalPath = dirPath;
    if (!_loc->getAutoIndex()){
        setError(FORBIDDEN, "handleGetMethod : autoIndex is off");
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
    if (!file.is_open())
        return false;
    std::ostringstream ss;
    ss << file.rdbuf();
    _body = ss.str();
    _statusCode = OK;
    _headers["Content-Type"] = extractContentType();
    if (_session && _headers["Content-Type"] == "text/html")
        applySessionTemplate(_body);
    return true;
}

/**
 * @brief Generates an HTML directory listing for _internalPath.
 */
void Response::displayAutoIndex(){
    DIR *dir = opendir(_internalPath.c_str());
    if (dir == NULL){
        setSyscallError("displayAutoIndex: dir is NULL");
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
        setError(BAD_REQUEST, "handlePostMethod: body can't be upload without spcifying file in URL");
        return;
    }
    if (request.getHeader("content-type").find("multipart/form-data") != std::string::npos){
        setError(FORBIDDEN, "handlePostMethod : multipart content-type not allowed without cgi");
        return ;
    }
    std::string uploadDir = _loc->getRoot();
    if(!uploadDir.empty() && uploadDir[uploadDir.size() - 1] == '/')
        uploadDir = uploadDir.substr(0, uploadDir.size() - 1);
    struct stat dirStat;
    if (stat(uploadDir.c_str(), &dirStat) != 0 || !S_ISDIR(dirStat.st_mode)){
        setSyscallError("upload folder doesnt exist");
        return ;
    }
    std::string uploadPath = uploadDir + "/" + _remain;
    LOG_INFO("uploadDir='" << uploadDir << "' uploadPath='" << uploadPath << "'");
    struct stat fileStat;
    if (stat(uploadPath.c_str(), &fileStat) == 0){
        setError(CONFLICT, "handlePostMethod: file already exists");
        return ;
    }
    std::ofstream file(uploadPath.c_str());
    if (!file.is_open()) {
        setSyscallError("handlePostMethod: can't open file");
        return;
    }
    file << request.getBody();
    file.close();
    _body = request.getBody();
    _statusCode = CREATED;
    _headers["Content-Type"] = request.getHeader("content-type");
}

/**
 * @brief Handles DELETE requests. Removes the target file.
 */
void Response::handleDeleteMethod(Request& request){
    struct stat sb;
   
    LOG_INFO("internal path is : " << _internalPath);
    if (stat(_internalPath.c_str(), &sb) != 0){
        setSyscallError("handleDeleteMethod : internalPath is not correct");
        return;
    }
    if (S_ISDIR(sb.st_mode)){
        if (request.getPath()[request.getPath().size() - 1] != '/'){
            setError(CONFLICT, "handleDeleteMethod: is a DIR with a wrong path synthax");
            return ;
        }
    }
    if (std::remove(_internalPath.c_str()) != 0){
        setSyscallError("handleDeleteMethod: function remove failed");
        return;
    }
    _statusCode = NO_CONTENT;
}

