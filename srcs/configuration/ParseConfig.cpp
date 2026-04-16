#include "../../include/ParseConfig.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "../../include/Http.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Location.hpp"
#include "../../include/color.hpp"

ParseConfig::ParseConfig() : _context(0) {
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename) : _filename(filename), _context(0), _http(0), _currentServer(0){
	std::cout << "ParseConfig: Normal construtor called" << std::endl;
}

ParseConfig::~ParseConfig() {
	std::cout << "ParseConfig: Destructor called" << std::endl;
}

int	ParseConfig::parsingConfigFile(std::vector<ServerConfig*> &serversConfigs) {
	this->openFile();
	while (processToken(serversConfigs)  == 0)
		;
	return (0);
}

int		ParseConfig::openFile(void) {
	std::string		buffer;
	std::ifstream	fileStream;

	errno = 0;
	fileStream.open(this->_filename.c_str());
	if (!fileStream.is_open())
	{
		std::cerr << C_RED << "Unable to open the file " << this->_filename << " :" << strerror(errno) << RESET << std::endl;
		return (errno);
	}
	while (getline(fileStream, buffer))
	{
		buffer += "\n";
		this->_configFile += buffer;
	}
	return (0);
}

std::string	ParseConfig::getNextToken(void) {

	std::string					token;
	std::vector<ServerConfig *>	servers;
	size_t						pos;

	pos = this->_configFile.find_first_not_of(" \n");
	this->_configFile.erase(this->_configFile.begin(), this->_configFile.begin() + pos);
	pos = this->_configFile.find_first_of(" \n");	//WARN: no protect for end, use find() to check the npos
	std::cout << pos << std::endl;
	token = this->_configFile.substr(0, pos);
	this->_configFile.erase(this->_configFile.begin(), this->_configFile.begin() + pos);

	std::cout << C_PINK << token << RESET << std::endl;
	return (token);
}

int	ParseConfig::processToken(std::vector<ServerConfig *> &serverConfigs) {
	int			errorCode;
	std::string	currentToken;
	
	errno = 0;
	// if (currentToken == "")
	// {
	// 	std::cout << C_GREEN << "End of configFile: no error" << RESET << std::endl;
	// 	return (1);
	// }

	currentToken = this->getNextToken();
	if (currentToken == "}") {
		this->_context -= 1;
		errorCode = 0;
	}
	
	else if (currentToken == "http")
		errorCode = this->processTokenHttp();

	else if (currentToken == "server") {
		errorCode = this->processServerToken(serverConfigs);
	}

	else if (currentToken == "location") {
		errorCode = this->processLocationToken();
	}

	else if (currentToken == "client_header_timeout") {
		errorCode = this->processHeaderTimeoutToken();
	}

	else if (currentToken == "client_body_timeout") {
		errorCode = this->processBodyTimeoutToken();
	}

	else if (currentToken == "client_max_body_size") {
		errorCode = this->processMaxBodySizeToken();
	}

	else if (currentToken == "root") {
		errorCode = this->processRootToken();
	}

	else if (currentToken == "listen") {
		errorCode = this->processListenToken();
	}

	else if (currentToken == "autoindex") {
		errorCode = this->processAutoindexToken();
	}

	else if (currentToken == "error_page") {
		errorCode = this->processErrorPageToken();
	}

	else {
		std::cerr << C_RED << currentToken << ": Unknow token" << RESET << std::endl;
		errorCode = 1;
	}
	return (errorCode);
}

int	ParseConfig::processTokenHttp(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	if (nextToken != "{" || this->_context!= NONE || this->_http) {
		std::cerr << C_RED << "Unknow token after http" << RESET << std::endl; 
		return (1);
	}
	try {
		this->_http = new Http();
	}
	catch (std::exception &e) {
		std::cerr << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
	}
	this->_context = HTTP;
	return (0);
}

int ParseConfig::processServerToken(std::vector<ServerConfig *> &servers) {
		std::string	nextToken;

		nextToken = getNextToken();
		if (nextToken != "{" || this->_context != HTTP || this->_currentServer) {
			std::cerr << C_RED << "TEST" << RESET << std::endl;
			return (1);
		}
		try {
			servers.push_back(new ServerConfig(*(this)->_http));
		}
		catch (std::exception &e) {
			std::cerr << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
			return (1);
		}
		this->_context = SERVER;
		this->_currentServer = *(servers.rbegin());
		return (0);
}

int	ParseConfig::processLocationToken(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	std::cout << C_CORAL << "Inside location block" << RESET << std::endl;
	if (nextToken != "{" || this->_context!= SERVER || this->_location) {
		std::cerr << C_RED << "TEST" << RESET << std::endl;
		return (1);
	}
	try {
		this->_location = new Location(*(this)->_currentServer);
	}
	catch (std::exception &e) {
		std::cerr << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
		return (1);
	}
	this->_context = LOCATION;
	return (0);
}


/**
 * @detail client_header_timeout context: http, server
 */
int ParseConfig::processHeaderTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return (1);
	if (this->_context == HTTP) {
		this->_http->setHeaderTimeout(timeout);
	}
	else if (this->_context == SERVER) {
		this->_currentServer->setHeaderTimeout(timeout);
	}
	else {
		std::cerr << C_RED << "Using token \"client_header_timeout\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @detail client_body_timeout context: http, server, location
 */
int ParseConfig::processBodyTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return (1);
	if (this->_context == HTTP) {
		this->_http->setBodyTimeout(timeout);
	}
	else if (this->_context == SERVER) {
		this->_currentServer->setBodyTimeout(timeout);
	}
	else if (this->_context == LOCATION) {
		this->_location->setBodyTimeout(timeout);
	}
	else {
		std::cerr << C_RED << "Using token \"client_body_timeout\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @detail client_max_body_size context: http, server, location
 */
int ParseConfig::processMaxBodySizeToken(void) {
	int			maxBodySize;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((maxBodySize = ParseConfig::transformTokenToInt(nextToken)) == -1) //FIX: need to mutiplie this later
		return (1);
	if (this->_context == HTTP) {
		this->_http->setMaxBodySize(maxBodySize);
	}
	else if (this->_context == SERVER) {
		this->_currentServer->setMaxBodySize(maxBodySize);
	}
	else if (this->_context == LOCATION) {
		this->_location->setMaxBodySize(maxBodySize);
	}
	else {
		std::cerr << C_RED << "Using token \"client_body_timeout\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @detail root context: http, server, location
 */
int ParseConfig::processRootToken(void) {
	std::string nextToken;

	nextToken = getNextToken();
	if (this->_context == HTTP){
		this->_http->setRoot(nextToken);
	}
	else if (this->_context == SERVER) {
		this->_currentServer->setRoute(nextToken);
	}
	else if (this->_context == LOCATION) {
		this->_location->setRoot(nextToken);
	}
	else {
		std::cerr << C_RED << "Using token \"root\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @detail listen context: server
 */
int ParseConfig::processListenToken(void) {
		int 		port;
		std::string	nextToken;

		nextToken = getNextToken();
		if (this->_context != SERVER)
		{
			std::cerr << C_RED << "Using token \"listen\" in wrong context" << RESET << std::endl;
			return (1);
		}
		if ((port = ParseConfig::transformTokenToInt(nextToken)) == -1)
			return (1);
		this->_currentServer->setPort(port);
		return (0);
}

/**
 * @detail autoindex context: http, server, location
 * @detail only response possible on | off
 */
int ParseConfig::processAutoindexToken(void) {
	bool		is_enable;
	std::string value;

	value = getNextToken();
	if (value == "on")
		is_enable = true;
	else if (value == "off")
		is_enable = false;
	else
	{
		std::cerr << C_RED << "value " << value << " unrecognized after autoindex" << RESET << std::endl;
		return (1);
	}
	if (this->_context == HTTP)
		this->_http->setAutoIndex(is_enable);
	else if (this->_context == SERVER)
		this->_currentServer->setAutoIndex(is_enable);
	else if (this->_context == LOCATION)
		this->_location->setAutoindex(is_enable);
	else {
		std::cout << C_RED << "Using token \"autoindex\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @detail error_page context: http, server, location
 */
int ParseConfig::processErrorPageToken(void) {
	int			code;
	std::string	codeToken, pageToken;

	codeToken = getNextToken();
	pageToken = getNextToken();
	std::stringstream ss(codeToken);
	if ((code = ParseConfig::transformTokenToInt(codeToken) == -1))
		return (1);
	if (this->_context == HTTP)
		this->_http->addErrorPage(code, pageToken);
	else if (this->_context == SERVER)
		this->_currentServer->addErrorPage(code, pageToken);
	else if (this->_context == LOCATION)
		this->_location->addErrorPage(code, pageToken);
	else {
		std::cout << C_RED << "Using token \"error_page\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

/**
 * @brief transform a token to a usable int
 * @param the token you want to convert
 * @return the converted int or -1 if an error occurs 
 */
int	ParseConfig::transformTokenToInt(std::string token) {
	int					nb;
	std::stringstream	ss(token);
	
	ss >> nb;
	if (ss.fail() || !ss.eof() || nb < 0)
	{
		std::cerr << C_RED << "invalid number in line x" << RESET << std::endl;
		return (-1);
	}
	std::cout << C_PURPLE << "DEBUG: number is: " << nb << RESET << std::endl;
	return (nb);
}

