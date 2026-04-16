#include "../include/ServerConfig.hpp"
#include <iostream>
#include "../include/color.hpp"
#include "../include/Http.hpp"

ServerConfig::ServerConfig() :
	_port(0),
	_serverName(""),
	_locations(0),
	_autoindex(false),
	_route(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexs(0) { //WARN: add the default "index.html" if this is empty after parsing
	std::cout << C_CSTR << "ServerConfig: Default constructor called" << RESET << std::endl;
	return ;
}

ServerConfig::ServerConfig(const Http &http) :
	_port(0),
	_serverName(""),
	_locations(0),
	_autoindex(http.getAutoIndex()),
	_route(http.getRoot()),
	_allowedMethods(http.getAllowedMethods()),
	_indexs(http.getIndexs()) {
	std::cout << C_CSTR << "ServerConfig: Normal constructor called" << RESET << std::endl;
	return ;
}

ServerConfig::~ServerConfig() {
	std::cout << C_DSTR << "ServerConfig: Destructor called" << RESET << std::endl;
	return ;
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

void ServerConfig::setAutoIndex(bool autoindex) {
	this->_autoindex = autoindex;
}

void ServerConfig::setRoute(const std::string& route) {
	this->_route = route;
}

void ServerConfig::setAllowedMethods(const std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void ServerConfig::addIndex(const std::string index) {
	this->_indexs.push_back(index);
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

bool ServerConfig::getAutoIndex(void) const {
	return (this->_autoindex);
}

std::string ServerConfig::getRoute(void) const {
	return (this->_route);
}

std::string ServerConfig::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> ServerConfig::getIndexs(void) const {
	return (this->_indexs);
}

