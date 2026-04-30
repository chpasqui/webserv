#include "../../include/Response.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"
#include "../../include/Cgi.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>

/**
 * @brief Builds the HTTP response from a parsed request.
 * Resolves location, checks method, dispatches to handler or CGI.
 */
void Response::build(Request& request, const ServerConfig *config){
	_config = config;
	_loc = findLocation(request.getPath());
	if (_loc == NULL){
		setError(NOT_FOUND);
		return;
	}
	if (request.getStatusCode() != 0){
		std::cout << "inside ERROR" << std::endl;
		setError(request.getStatusCode());
		return ;
	}
	 //si on met dans serverConfig config->findLocation
	_internalPath = buildFileSystemPath(request.getPath());
	std::cout << "internal path is " << _internalPath << std::endl;
	
	if (_loc->getRedirCode() != 0){
		_statusCode = _loc->getRedirCode();
		_headers["Location"] = _loc->getRedirPath();
		return ;
	}
	if (!methodAllowed(request.getMethod())){
		setError(METHOD_NOT_ALLOWED);
		return ;
	}
	 if (isCGI())
		std::cout << "inside the build function, isCgi is set to true" << std::endl;
	// 	this->_isCgi = true;
	// 	return;
	// }
	handleMethod(request);
}

/**
 * @brief Returns the longest-prefix matching location for the given URL.
 * @return Pointer to matching Location, or NULL if none found.
 */
Location* Response::findLocation(const std::string& url){
	Location *best = NULL;
	size_t bestLen = 0;

	std::vector<Location*> locs = _config->getLocations();
	std::vector<Location*>::iterator it = locs.begin();
	for (; it != locs.end(); it++){
		const std::string& prefix = (*it)->getPath();
		if (url.substr(0, prefix.size()) == prefix && prefix.size() > bestLen){
			best = *it;
			bestLen = prefix.size();
		}
	}
	return best;
}

/**
 * @brief Builds the filesystem path from location root and URL remainder.
 */
std::string Response::buildFileSystemPath(const std::string& url){    
	_remain = url.substr(1);
    return _loc->getRoot() + _remain;
}

/**
 * @brief Checks if the method is allowed by the matched location.
 */
bool Response::methodAllowed(const std::string& method){
	std::string methodAllowed = _loc->getAllowedMethods();
	if (methodAllowed.find(method) != std::string::npos)
		return true;
	return false;
}

/**
 * @brief Checks if the request targets a CGI resource by extension.
 */
bool Response::isCGI(){
	std::map<std::string, std::string> temp = this->_loc->getCgiExtensions();

	if (!temp.empty()){
		std::map<std::string, std::string>::iterator it = temp.begin();
		for (; it != temp.end(); it++){
			std::cout << it->first << std::endl;
			size_t pos = _internalPath.find(it->first); 
			if (pos != std::string::npos){
				size_t end = pos + it->first.size();
				if (end == _internalPath.size() || _internalPath[end] == '/'){
					_cgiPass = it->second;
					return true;
				}
			}
		}
		return false;
	}
	return false;
}

/**
 * @brief Dispatches to the appropriate method handler (GET/POST/DELETE).
 */
void Response::handleMethod(Request& request){
	if (request.getMethod() == "GET")
		handleGetMethod(request);
	else if (request.getMethod() == "POST"){
		if (request.getPath() == "/login")
			handleLogin(request);
		else
			handlePostMethod(request);
	}
	else
		handleDeleteMethod(request);
}

/**
 * @brief Sets _statusCode from errno and calls buildError().
 */
void Response::setSyscallError(){
	if (errno == ENOENT || errno == ENOTDIR)
		setError(NOT_FOUND);
	else if (errno == EACCES)
		setError(FORBIDDEN);
	else
		setError(INTERNAL_SERVER_ERROR);
}

/**
 * @brief Populates _body with a custom or generated error page.
 */
void Response::buildError(){

	std::string errorPageFile;
	if (_loc && !_loc->getErrorPage(_statusCode).empty())
		errorPageFile = _loc->getRoot() + "/" + _loc->getErrorPage(_statusCode);
	// else if (!_config->getErrorPage(_statusCode).empty())
	//     errorPageFile = _config->getRoot() + "/" + _config->getErrorPage(_statusCode);
	if (!errorPageFile.empty()){
		struct stat errorPath;
		if (stat(errorPageFile.c_str(), &errorPath) == 0 && S_ISREG(errorPath.st_mode)){
			std::ifstream file(errorPageFile.c_str());
			if (file.is_open()){
				std::ostringstream ss;
				ss << file.rdbuf();
				_body = ss.str();
				_headers["Content-Type"] = "text/html";
				return ;
			}
		}
	}
	generateErrorPage();
}

/**
 * @brief Generates a minimal HTML error page from _statusCode.
 */
void Response::generateErrorPage()
{
	std::ostringstream body;

	body << "<html>\n<head><title>" << _statusCode << " " << getReasonPhrase() << "</title></head>\n"
		 << "<body>\n"
		 << "<h1>" << _statusCode << " " << getReasonPhrase() << "</h1>\n"
		 << "<hr>\n"
		 << "<p>webserv</p>\n"
		 << "</body>\n</html>\n";

	_body = body.str();
	_headers["Content-Type"] = "text/html";
}

