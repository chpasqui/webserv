#include "../../include/Http.hpp"
#include "../../include/Debug.hpp"
#include "../../include/color.hpp"

Http::Http() :
	_clientTimeout(30),
	_maxBodysize(1000), //WARN: check the correct convertion of 1m in bit
	_autoindex(true),
	_root(""),
	_allowedMethods("GET|POST|DELETE"),
	_indexes(0),
	_errorPages() {
	 LOG_DEBUG(C_CSTR << "Http: Default constructor called" << RESET);
	return ;
}

Http::~Http() {
	LOG_DEBUG(C_DSTR << "Http: Destructor called" << RESET);
	return ;
}

//----Getter----//

int Http::getClientTimeout(void) const {
	return (_clientTimeout);
}

int Http::getMaxBodySize(void) const {
	return (_maxBodysize);
}

bool Http::getAutoindex(void) const {
	return (_autoindex);
}

std::string	Http::getRoot(void) const {
	return (_root);
}

std::string Http::getAllowedMethods(void) const {
	return (_allowedMethods);
}

std::vector<std::string> Http::getIndexes(void) const {
	return (_indexes);
}

std::map<int, std::string> Http::getErrorPages(void) const
{
	return (_errorPages);
}

//----Setter----//

void Http::setClientTimeout(int clientTimeout) {
	_clientTimeout = clientTimeout;
}

void Http::setMaxBodySize(int maxBodySize) {
	_maxBodysize = maxBodySize;
}

void Http::setAutoindex(bool autoindex) {
	_autoindex = autoindex;
}

void Http::setRoot(std::string root) {
	_root = root;
}

void Http::setAllowedMethods(std::string allowedMethods) {
	_allowedMethods = allowedMethods;
}

void Http::addIndex(std::string index) {
	_indexes.push_back(index);
}

void Http::addErrorPage(int code, std::string page) {
	_errorPages.insert(std::map<int, std::string>::value_type(code, page));
}
