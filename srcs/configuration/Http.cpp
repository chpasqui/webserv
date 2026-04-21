#include "../../include/Http.hpp"

#include <iostream>
#include "../../include/color.hpp"

Http::Http() :
	_headerTimeout(30),
	_bodyTimeout(30),
	_maxBodysize(1000), //WARN: check the correct convertion of 1m in bit
	_autoindex(true),
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexes(0),
	_errorPages() {
	std::cout << C_CSTR << "Http: Default constructor called" << RESET << std::endl;
	return ;
}

Http::~Http() {
	std::cout << C_DSTR << "Http: Destructor called" << RESET << std::endl;
	return ;
}

//----Getter----//

int Http::getHeaderTimeout(void) const {
	return (this->_headerTimeout);
}

int Http::getBodyTimeout(void) const {
	return (this->_bodyTimeout);
}

int Http::getMaxBodySize(void) const {
	return (this->_maxBodysize);
}

bool Http::getAutoindex(void) const {
	return (this->_autoindex);
}

std::string	Http::getRoot(void) const {
	return (this->_root);
}

std::string Http::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> Http::getIndexes(void) const {
	return (this->_indexes);
}

std::map<int, std::string> Http::getErrorPages(void) const
{
	return (this->_errorPages);
}

//----Setter----//

void Http::setHeaderTimeout(int headerTimeout) {
	this->_headerTimeout = headerTimeout;
}

void Http::setBodyTimeout(int bodyTimeout) {
	this->_bodyTimeout = bodyTimeout;
}

void Http::setMaxBodySize(int maxBodySize) {
	this->_maxBodysize = maxBodySize;
}

void Http::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}

void Http::setRoot(std::string root) {
	this->_root = root;
}

void Http::setAllowedMethods(std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void Http::addIndex(std::string index) {
	this->_indexes.push_back(index);
}

void Http::addErrorPage(int code, std::string page) {
	this->_errorPages.insert(std::map<int, std::string>::value_type(code, page));
}
