#include "../../include/AConfig.hpp"
#include <stdexcept>
#include "../../include/Debug.hpp"
#include "../../include/color.hpp"

AConfig::AConfig() {
	// LOG(C_CSTR << "AConfig: Default constructor called"<< RESET);
}

AConfig::~AConfig() {
	// LOG(C_DSTR << "AConfig: Destructor called" << RESET);
}

void	AConfig::setRedireCode(const int redireCode){
	(void) redireCode;
	throw (std::runtime_error("wrong context with \"return\" (redireCode)"));
}

void	AConfig::setRedirePath(const std::string redirePath) {
	(void) redirePath;
	throw (std::runtime_error("wrong context with \"return\" (redirePath)"));
}

void	AConfig::addCgiExtension(const std::string extension, const std::string path) {
	(void) extension;
	(void) path;
	throw (std::runtime_error("wrong context with \"cgi_pass\""));
}

void	AConfig::setClientTimeout(const int clientTimeout) {
	(void) clientTimeout;
	throw (std::runtime_error("wrong context with \"client_timeout\""));
}

void	AConfig::setMaxBodySize(const int maxBodySize) {
	(void) maxBodySize;
	throw (std::runtime_error("wrong context with \"client_max_body_size\""));
}

void	AConfig::setServerName(const std::string serverName) {
	(void) serverName;
	throw (std::runtime_error("wrong context with \"server_name\""));
}

void	AConfig::setPort(const int port) {
	(void) port;
	throw (std::runtime_error("wrong context with \"listen\""));
}

void	AConfig::setAutoindex(const bool autoindex) {
	(void) autoindex;
	throw (std::runtime_error("wrong context with \"autoindex\""));
}

void	AConfig::setRoot(const std::string root) {
	(void) root;
	throw (std::runtime_error("wrong context with \"root\""));
}

void	AConfig::setAllowedMethods(const std::string allowedMethods) {
	(void) allowedMethods;
	throw (std::runtime_error("wrong context with \"allowed_methods\""));
}

void	AConfig::addIndex(const std::string index) {
	(void) index;
	throw (std::runtime_error("wrong context with \"index\""));
}

void	AConfig::addErrorPage(const int code, const std::string page) {
	(void) code;
	(void) page;
	throw (std::runtime_error("wrong context with \"error_page\""));
}

