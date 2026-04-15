#include "../../include/HTTP.hpp"

HTTP::HTTP() :
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_autoindex(true),
	_indexs(0) {
	return ;
}

HTTP::~HTTP() {
	return ;
}

bool HTTP::getAutoIndex(void) const {
	return (this->_autoindex);
}

std::string	HTTP::getRoot(void) const {
	return (this->_root);
}

std::string HTTP::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> HTTP::getIndex(void) const {
	return (this->_indexs);
}

void HTTP::setAutoIndex(bool autoindex) {
	this->_autoindex = autoindex;
}

void HTTP::setRoot(std::string root) {
	this->_root = root;
}

void HTTP::setAllowedMethods(std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void HTTP::addIndex(std::string index) {
	this->_indexs.push_back(index);
}
