#include "../../include/Location.hpp"
#include "../../include/ServerConfig.hpp"
#include <string>

Location::Location() {
	return ;
}

Location::Location(const ServerConfig &serverConfig) :
	_redireCode(0),
	_cgiEnable(false),
	_path("/"),
	_redirePath(""),
	_cgiExtension(0),
	_port(serverConfig.getPort()),
	_autoindex(serverConfig.getAutoIndex()),
	_serverName(serverConfig.getServerName()),
	_root(serverConfig.getRoute()),
	_allowedMethods(serverConfig.getAllowedMethods()),
	_indexs(serverConfig.getIndexs()) {
	return ;
}

Location::~Location() {
	return ;
}

//----Setter----//

void Location::setRedireCode(const int redireCode) {
	this->_redireCode = redireCode;
}

void Location::setCgiEnable(const bool cgiEnable) {
	this->_cgiEnable = cgiEnable;
}

void Location::setPath(const std::string path) {
	this->_path = path;
}

void Location::setredirePath(const std::string redirePath) {
	this->_redirePath = redirePath;
}

void Location::addCgiExtension(const std::string extension) {
	this->_cgiExtension.push_back(extension);
}

void Location::setPort(const int port) {
	this->_port = port;
}

void Location::setAutoindex(const bool autoindex) {
	this->_autoindex = autoindex;
}

void Location::setRoot(const std::string root) {
	this->_root = root;
}

void Location::setAllowedMethods(const std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void Location::addIndex(const std::string index) {
	this->_indexs.push_back(index);
}

//----Getter----//

int Location::getRedirCode(void) const {
	return (this->_redireCode);
}

bool Location::getCgiEnable(void) const {
	return (this->_cgiEnable);
}

std::string Location::getPath(void) const {
	return (this->_path);
}

std::string Location::getRedirPath(void) const {
	return (this->_redirePath);
}

std::vector<std::string> Location::getCgiExtensions(void) const {
	return (this->_cgiExtension);
}

int Location::getPort(void) const {
	return (this->_port);
}

bool Location::getAutoIndex(void) const {
	return (this->_autoindex);
}

std::string Location::getServerName(void) const {
	return (this->_serverName);
}

std::string Location::getRoot(void) const {
	return (this->_root);
}

std::string Location::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> Location::getIndexs(void) const {
	return (this->_indexs);
}

