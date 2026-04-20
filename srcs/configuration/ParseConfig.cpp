#include "../../include/ParseConfig.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "../../include/Http.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Location.hpp"
#include "../../include/color.hpp"

ParseConfig::ParseConfig() :
	_filename(""),
	_context(0),
	_http(0),
	_currentConfig(0),
	_location(0),
	_dictionary() {
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename, std::vector<ServerConfig *> &serverConfigs) :
	_filename(filename),
	_context(0),
	_http(0),
	_currentConfig(0),
	_location(0),
	_serverConfigs(serverConfigs){
	_dictionary["}"] = &ParseConfig::processEndBraquetToken;
	_dictionary["http"] = &ParseConfig::processTokenHttp;
	_dictionary["server"] = &ParseConfig::processServerToken;
	_dictionary["location"] = &ParseConfig::processLocationToken;
	_dictionary["client_header_timeout"] = &ParseConfig::processHeaderTimeoutToken;
	_dictionary["client_body_timeout"] = &ParseConfig::processBodyTimeoutToken;
	_dictionary["client_max_body_size"] = &ParseConfig::processMaxBodySizeToken;
	_dictionary["root"] = &ParseConfig::processRootToken;
	_dictionary["listen"] = &ParseConfig::processListenToken;
	_dictionary["autoindex"] = &ParseConfig::processAutoindexToken;
	_dictionary["error_page"] = &ParseConfig::processErrorPageToken;

	std::cout << "ParseConfig: Normal construtor called" << std::endl;
}

ParseConfig::~ParseConfig() {
	std::cout << "ParseConfig: Destructor called" << std::endl;
}

int	ParseConfig::parsingConfigFile(void) {
	std::string													token;
	std::map<std::string, void (ParseConfig::*)(void)>::iterator	it;

	errno = 0;
	this->openFile();
	while (true) 
	{
		token = this->getNextToken();
		// if (currentToken == "")
		// {
		// 	std::cout << C_GREEN << "End of configFile: no error" << RESET << std::endl;
		// 	return (1);
		// }
		if (it == this->_dictionary.end())
		{
			std::cerr << C_RED << token << ": Unknow token" << RESET << std::endl; //TODO: Add line number
			return (1);
		}
		
		try {
			it = this->_dictionary.find(token);
			(this->*it->second)();
		}
		catch (std::exception &e) {
			std::cout << C_RED << e.what() << RESET << std::endl;
			return (1);
		}
	}
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

void	ParseConfig::processEndBraquetToken(void) {
	std::string currentToken;

	currentToken = this->getNextToken();
	if (currentToken == "}") {
		this->_context -= 1;
	}
}

void	ParseConfig::processTokenHttp(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	if (nextToken != "{" || this->_context!= NONE || this->_http) {
		std::cerr << C_RED << "Unknow token after http" << RESET << std::endl; 
		return;
	}
	try {
		this->_http = new Http();
	}
	catch (std::exception &e) {
		std::cerr << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
	}
	this->_context = HTTP;
}

void	ParseConfig::processServerToken(void) {
		std::string	nextToken;

		nextToken = getNextToken();
		if (nextToken != "{" || this->_context != HTTP || this->_currentConfig) {
			std::cerr << C_RED << "TEST" << RESET << std::endl;
			return;
		}
		this->_serverConfigs.push_back(new ServerConfig(*(this)->_http));
		this->_context = SERVER;
		this->_currentConfig = *(this->_serverConfigs.rbegin());
}

void	ParseConfig::processLocationToken(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	std::cout << C_CORAL << "Inside location block" << RESET << std::endl;
	if (nextToken != "{" || this->_context!= SERVER || this->_location) {
		std::cerr << C_RED << "TEST" << RESET << std::endl;
		return ;
	}
	this->_location = new Location(*(this)->_currentConfig);
	this->_context = LOCATION;
	return ;
}


/**
 * @detail client_header_timeout context: http, server
 */
void	ParseConfig::processHeaderTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return ;
	if (this->_context == HTTP) {
		this->_http->setHeaderTimeout(timeout);
	}
	else if (this->_context == SERVER) {
		this->_currentConfig->setHeaderTimeout(timeout);
	}
	else {
		std::cerr << C_RED << "Using token \"client_header_timeout\" in wrong context" << RESET << std::endl;
		return ;
	}
	return ;
}

/**
 * @detail client_body_timeout context: http, server, location
 */
void	ParseConfig::processBodyTimeoutToken(void) {
	int			timeout;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((timeout = ParseConfig::transformTokenToInt(nextToken)) == -1)
		return ;
	if (this->_context == HTTP) {
		this->_http->setBodyTimeout(timeout);
	}
	else if (this->_context == SERVER) {
		this->_currentConfig->setBodyTimeout(timeout);
	}
	else if (this->_context == LOCATION) {
		this->_location->setBodyTimeout(timeout);
	}
	else {
		std::cerr << C_RED << "Using token \"client_body_timeout\" in wrong context" << RESET << std::endl;
		return ;
	}
	return ;
}

/**
 * @detail client_max_body_size context: http, server, location
 */
void	ParseConfig::processMaxBodySizeToken(void) {
	int			maxBodySize;
	std::string	nextToken;

	nextToken = getNextToken();
	if ((maxBodySize = ParseConfig::transformTokenToInt(nextToken)) == -1) //FIX: need to mutiplie this later
		return ;
	if (this->_context == HTTP) {
		this->_http->setMaxBodySize(maxBodySize);
	}
	else if (this->_context == SERVER) {
		this->_currentConfig->setMaxBodySize(maxBodySize);
	}
	else if (this->_context == LOCATION) {
		this->_location->setMaxBodySize(maxBodySize);
	}
	else {
		std::cerr << C_RED << "Using token \"client_body_timeout\" in wrong context" << RESET << std::endl;
		return ;
	}
	return;
}

/**
 * @detail root context: http, server, location
 */
void	ParseConfig::processRootToken(void) {
	std::string nextToken;

	nextToken = getNextToken();
	if (this->_context == HTTP){
		this->_http->setRoot(nextToken);
	}
	else if (this->_context == SERVER) {
		this->_currentConfig->setRoute(nextToken);
	}
	else if (this->_context == LOCATION) {
		this->_location->setRoot(nextToken);
	}
	else {
		std::cerr << C_RED << "Using token \"root\" in wrong context" << RESET << std::endl;
		return;
	}
	return;
}

/**
 * @detail listen context: server
 */
void	ParseConfig::processListenToken(void) {
		int 		port;
		std::string	nextToken;

		nextToken = getNextToken();
		if (this->_context != SERVER)
		{
			std::cerr << C_RED << "Using token \"listen\" in wrong context" << RESET << std::endl;
			return;
		}
		if ((port = ParseConfig::transformTokenToInt(nextToken)) == -1)
			return;
		this->_currentConfig->setPort(port);
		return;
}

/**
 * @detail autoindex context: http, server, location
 * @detail only response possible on | off
 */
void	ParseConfig::processAutoindexToken(void) {
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
		return;
	}
	if (this->_context == HTTP)
		this->_http->setAutoIndex(is_enable);
	else if (this->_context == SERVER)
		this->_currentConfig->setAutoIndex(is_enable);
	else if (this->_context == LOCATION)
		this->_location->setAutoindex(is_enable);
	else {
		std::cout << C_RED << "Using token \"autoindex\" in wrong context" << RESET << std::endl;
		return;
	}
	return;
}

/**
 * @detail error_page context: http, server, location
 */
void	ParseConfig::processErrorPageToken(void) {
	int			code;
	std::string	codeToken, pageToken;

	codeToken = getNextToken();
	pageToken = getNextToken();
	std::stringstream ss(codeToken);
	if ((code = ParseConfig::transformTokenToInt(codeToken) == -1))
		return;
	if (this->_context == HTTP)
		this->_http->addErrorPage(code, pageToken);
	else if (this->_context == SERVER)
		this->_currentConfig->addErrorPage(code, pageToken);
	else if (this->_context == LOCATION)
		this->_location->addErrorPage(code, pageToken);
	else {
		std::cout << C_RED << "Using token \"error_page\" in wrong context" << RESET << std::endl;
		return;
	}
	return;
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

