#include "../../include/Location.hpp"
#include "../../include/color.hpp"
#include "../../include/debug.hpp"
#include "../../include/ServerConfig.hpp"

Location::Location() {
	LOG(C_CSTR << "Location: Default constructor called" << RESET);
	return ;
}

Location::Location(const ServerConfig &serverConfig) :
	_redireCode(0),
	_cgiEnable(false),
	_path("/"),
	_redirePath(""),
	_cgiExtension(0),
	_headerTimeout(serverConfig.getHeaderTimeout()),
	_bodyTimeout(serverConfig.getBodyTimeout()),
	_maxBodysize(serverConfig.getMaxBodySize()),
	_port(serverConfig.getPort()),
	_autoindex(serverConfig.getAutoindex()),
	_serverName(serverConfig.getServerName()),
	_root(serverConfig.getRoute()),
	_allowedMethods(serverConfig.getAllowedMethods()),
	_indexes(serverConfig.getIndexes()) {
	LOG(C_CSTR << "Location: Normal constructor called" << RESET);
	return ;
}

Location::~Location() {
	LOG(C_DSTR << "Location: Destructor called" << RESET);
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

void Location::setRedirePath(const std::string redirePath) {
	this->_redirePath = redirePath;
}

void Location::addCgiExtension(const std::string extension) {
	this->_cgiExtension.push_back(extension);
}

void Location::setBodyTimeout(const int bodyTimeout) {
	this->_bodyTimeout = bodyTimeout;
}

void Location::setMaxBodySize(const int maxBodySize) {
	this->_maxBodysize = maxBodySize;
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
	this->_indexes.push_back(index);
}

void Location::addErrorPage(const int code, const std::string page) {
	this->_errorPages.insert(std::map<int, std::string>::value_type(code, page));
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

int Location::getHeaderTimeout(void) const {
	return (this->_headerTimeout);
}

int Location::getBodyTimeout(void) const {
	return (this->_bodyTimeout);
}

int Location::getMaxBodySize(void) const {
	return (this->_maxBodysize);
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

std::vector<std::string> Location::getIndexes(void) const {
	return (this->_indexes);
}

std::string Location::getUploadStore() const{
	return (this->_uploadStore);
}

std::string Location::getErrorPage(int code) const
{
    std::map<int, std::string>::const_iterator it = _errorPages.find(code);
    if (it != _errorPages.end())
        return it->second;
    return "";
}

