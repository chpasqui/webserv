#include "../../include/AConfig.hpp"
#include <stdexcept>
#include "../../include/color.hpp"
#include "../../include/Debug.hpp"

AConfig::AConfig() {
	LOG(C_CSTR << "AConfig: Default constructor called"<< RESET);
}

AConfig::~AConfig() {
	LOG(C_DSTR << "AConfig: Destructor called" << RESET);
}

void	AConfig::setRedireCode(const int redireCode){
	(void) redireCode;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setCgiEnable(const bool cgiEnable){
	(void) cgiEnable;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setPath(const std::string path) {
	(void) path;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setredirePath(const std::string redirePath) {
	(void) redirePath;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::addCgiExtension(const std::string extension) {
	(void) extension;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setHeaderTimeout(const int headerTimeout) {
	(void) headerTimeout;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setBodyTimeout(const int bodyTimeout) {
	(void) bodyTimeout;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setMaxBodySize(const int maxBodySize) {
	(void) maxBodySize;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setPort(const int port) {
	(void) port;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setAutoindex(const bool autoindex) {
	(void) autoindex;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setRoot(const std::string root) {
	(void) root;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::setAllowedMethods(const std::string allowedMethods) {
	(void) allowedMethods;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::addIndex(const std::string index) {
	(void) index;
	throw (std::runtime_error("wrong context"));
}

void	AConfig::addErrorPage(const int code, const std::string page) {
	(void) code;
	(void) page;
	throw (std::runtime_error("wrong context"));
}

