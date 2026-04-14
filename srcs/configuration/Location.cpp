#include "../../include/Location.hpp"
#include "../../include/ServerConfig.hpp"

Location::Location() {
	return ;
}

Location::Location(ServerConfig serverConfig) :
	_root(serverConfig.getRoute()),
	_autoindex(serverConfig.getAutoIndex()),
	_allowedMethods(serverConfig.getAllowedMethods()),
	_path(""),
	_cgiEnable(false),
	_redireCode(0),
	_redirePath(0) {
	return ;
}

Location::~Location() {
	return ;
}

void	Location::setRoot(std::string root) {
	this->_root = root;
}

void	Location::getAllowedMethods(std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void	Location::setPath(std::string path) {
	this->_path = path;
}

void	Location::setredirePath(std::string redirePath) {
	this->_redirePath = redirePath;
}

void	Location::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}

void	Location::setCgiEnable(bool cgiEnable) {
	this->_cgiEnable = cgiEnable;
}

void	Location::setRedireCode(int redireCode) {
	this->_redireCode = redireCode;
}


