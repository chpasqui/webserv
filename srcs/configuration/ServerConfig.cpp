#include "../../include/ServerConfig.hpp"
#include "../../include/Debug.hpp"
#include "../../include/color.hpp"
#include "../../include/Http.hpp"
#include "../../include/Location.hpp"


ServerConfig::ServerConfig() :
	_port(0),
	_serverName(""),
	_locations(0),
	_headerTimeout(0),
	_bodyTimeout(0),
	_maxBodysize(0),
	_autoindex(false),
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexes(0),
	_errorPages() { //WARN: add the default "index.html" if this is empty after parsing
	LOG(C_CSTR << "ServerConfig: Default constructor called" << RESET);
	return ;
}

ServerConfig::ServerConfig(const Http &http) :
	_port(0),
	_serverName(""),
	_locations(0),
	_headerTimeout(http.getHeaderTimeout()),
	_bodyTimeout(http.getBodyTimeout()),
	_maxBodysize(http.getMaxBodySize()),
	_autoindex(http.getAutoindex()),
	_root(http.getRoot()),
	_allowedMethods(http.getAllowedMethods()),
	_indexes(http.getIndexes()),
	_errorPages(http.getErrorPages()) {
	LOG(C_CSTR << "ServerConfig: Normal constructor called" << RESET);
	return ;
}

ServerConfig::~ServerConfig(){
	for (std::vector<Location *>::iterator it = this->_locations.begin(); it != this->_locations.end(); it++)
		delete (*it);
	this->_locations.clear();
	return ;
	LOG(C_DSTR << "ServerConfig: Destructor called" << RESET);
}

//----Setter---//

void ServerConfig::setPort(const int port) {
	this->_port = port;
}

void ServerConfig::setServerName(const std::string serverName) {
	this->_serverName = serverName;
}

void ServerConfig::addLocation(Location *location) {
	this->_locations.push_back(location);
}

void ServerConfig::setHeaderTimeout(int headerTimeout) {
	this->_headerTimeout = headerTimeout;
}

void ServerConfig::setBodyTimeout(int bodyTimeout) {
	this->_bodyTimeout = bodyTimeout;
}

void ServerConfig::setMaxBodySize(int maxBodySize) {
	this->_maxBodysize = maxBodySize;
}

void ServerConfig::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}

void ServerConfig::setRoot(const std::string route) {
	this->_root = route;
}

void ServerConfig::setAllowedMethods(const std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void ServerConfig::addIndex(const std::string index) {
	this->_indexes.push_back(index);
}

std::map<int, std::string> ServerConfig::getErrorPages(void) const
{
	return (this->_errorPages);
}

//----Getter----//

int	ServerConfig::getPort(void) const {
	return (this->_port);
}

std::string ServerConfig::getServerName(void) const {
	return (this->_serverName);
}

std::vector<Location *> ServerConfig::getLocations(void) const {
	return (this->_locations);
}

int ServerConfig::getHeaderTimeout(void) const {
	return (this->_headerTimeout);
}

int ServerConfig::getBodyTimeout(void) const {
	return (this->_bodyTimeout);
}

int ServerConfig::getMaxBodySize(void) const {
	return (this->_maxBodysize);
}

bool ServerConfig::getAutoindex(void) const {
	return (this->_autoindex);
}

std::string ServerConfig::getRoute(void) const {
	return (this->_root);
}

std::string ServerConfig::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> ServerConfig::getIndexes(void) const {
	return (this->_indexes);
}

void ServerConfig::addErrorPage(int code, std::string page) {
	this->_errorPages.insert(std::map<int, std::string>::value_type(code, page));
}

