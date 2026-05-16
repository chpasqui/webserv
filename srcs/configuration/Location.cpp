#include "../../include/Location.hpp"
#include "../../include/color.hpp"
#include "../../include/Debug.hpp"
#include "../../include/ServerConfig.hpp"

Location::Location() :
	_redireCode(0),
	_path("/"),
	_redirePath(""),
	_cgiExtensions(),
	_clientTimeout(0),
	_maxBodysize(0),
	_port(0),
	_autoindex(false),
	_serverName(""),
	_root(""),
	_allowedMethods(""),
	_indexes(),
	_errorPages() {
	LOG_DEBUG(C_CSTR << "Location: Default constructor called");
	return ;
}

Location::Location(const ServerConfig &serverConfig, const std::string path) :
	_redireCode(0),
	_path(path),
	_redirePath(""),
	_cgiExtensions(),
	_clientTimeout(serverConfig.getClientTimeout()),
	_maxBodysize(serverConfig.getMaxBodySize()),
	_port(serverConfig.getPort()),
	_autoindex(serverConfig.getAutoindex()),
	_serverName(serverConfig.getServerName()),
	_root(serverConfig.getRoute()),
	_allowedMethods(serverConfig.getAllowedMethods()),
	_indexes(serverConfig.getIndexes()) {
	_errorPages = serverConfig.getErrorPages();
	LOG_DEBUG(C_CSTR << "Location: Normal constructor called");
	return ;
}

Location::~Location() {
	LOG_DEBUG(C_DSTR << "Location: Destructor called");
	return ;
}

//----Setter----//

void Location::setRedireCode(const int redireCode) {
	_redireCode = redireCode;
}

void Location::setRedirePath(const std::string redirePath) {
	_redirePath = redirePath;
}

void Location::addCgiExtension(const std::string extension, const std::string path) {
	_cgiExtensions.insert(std::map<std::string, std::string>::value_type(extension, path));
}

void Location::setMaxBodySize(const int maxBodySize) {
	_maxBodysize = maxBodySize;
}

void Location::setAutoindex(const bool autoindex) {
	_autoindex = autoindex;
}

void Location::setRoot(const std::string root) {
	_root = root;
}

void Location::setAllowedMethods(const std::string allowedMethods) {
	_allowedMethods = allowedMethods;
}

void Location::addIndex(const std::string index) {
	_indexes.push_back(index);
}

void Location::addErrorPage(const int code, const std::string page) {
	_errorPages.insert(std::map<int, std::string>::value_type(code, page));
}

//----Getter----//

int Location::getRedirCode(void) const {
	return (_redireCode);
}

std::string Location::getPath(void) const {
	return (_path);
}

std::string Location::getRedirPath(void) const {
	return (_redirePath);
}

std::map<std::string, std::string> Location::getCgiExtensions(void) const {
	return (_cgiExtensions);
}

int Location::getMaxBodySize(void) const {
	return (_maxBodysize);
}

int Location::getPort(void) const {
	return (_port);
}

bool Location::getAutoIndex(void) const {
	return (_autoindex);
}

std::string Location::getServerName(void) const {
	return (_serverName);
}

std::string Location::getRoot(void) const {
	return (_root);
}

std::string Location::getAllowedMethods(void) const {
	return (_allowedMethods);
}

std::vector<std::string> Location::getIndexes(void) const {
	return (_indexes);
}

std::string Location::getErrorPage(int code) const
{
    std::map<int, std::string>::const_iterator it = _errorPages.find(code);
    if (it != _errorPages.end())
        return it->second;
    return "";
}

