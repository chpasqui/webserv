#include "../../include/Http.hpp"

Http::Http() :
	_autoindex(true),
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexs(0) {
	return ;
}

Http::~Http() {
	return ;
}

//----Getter----//

bool Http::getAutoIndex(void) const {
	return (this->_autoindex);
}

std::string	Http::getRoot(void) const {
	return (this->_root);
}

std::string Http::getAllowedMethods(void) const {
	return (this->_allowedMethods);
}

std::vector<std::string> Http::getIndexs(void) const {
	return (this->_indexs);
}

//----Setter----//

void Http::setAutoIndex(bool autoindex) {
	this->_autoindex = autoindex;
}

void Http::setRoot(std::string root) {
	this->_root = root;
}

void Http::setAllowedMethods(std::string allowedMethods) {
	this->_allowedMethods = allowedMethods;
}

void Http::addIndex(std::string index) {
	this->_indexs.push_back(index);
}
