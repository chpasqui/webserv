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
#include "../../include/Debug.hpp"

ParseConfig::ParseConfig() :
	_filename(""),
	_contextLevel(0),
	_http(0),
	_currentConfig(0),
	_contextConfig(0),
	_location(0),
	_serverConfigs(0),
	_dictionary() {
	LOG("ParseConfig: Default constructor called");
}

ParseConfig::ParseConfig(std::string filename, std::vector<ServerConfig *> *serverConfigs) :
	_filename(filename),
	_contextLevel(0),
	_http(0),
	_currentConfig(0),
	_contextConfig(0),
	_location(0),
	_serverConfigs(serverConfigs) {
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
	LOG("ParseConfig: Normal construtor called");
}

ParseConfig::~ParseConfig() {
	LOG("ParseConfig: Destructor called");
}

int	ParseConfig::parsingConfigFile(void) {
	std::string														token;
	std::map<std::string, void (ParseConfig::*)(void)>::iterator	it;

	errno = 0;
	this->openFile();
	while (true) 
	{
		token = this->getNextToken();
		if (token == "")
		{
			LOG(C_GREEN << "End of configFile: no error" << RESET);
			return (1);
		}
		it = this->_dictionary.find(token);
		if (it == this->_dictionary.end())
		{
			std::cerr << C_RED << token << ": Unknow token" << RESET  << std::endl; //TODO: Add line number
			return (1);
		}
		
		try {
			LOG(token);
			(this->*it->second)();
			LOG(" no segfault");
		}
		catch (std::exception &e) {
			LOG(C_RED << e.what() << RESET);
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
	LOG(pos);
	token = this->_configFile.substr(0, pos);
	this->_configFile.erase(this->_configFile.begin(), this->_configFile.begin() + pos);

	LOG(C_PINK << token << RESET);
	return (token);
}

void	ParseConfig::processEndBraquetToken(void) {
	std::string currentToken;

	currentToken = this->getNextToken();
	if (currentToken == "}") {
		this->_contextLevel -= 1;
	}
}

void	ParseConfig::processTokenHttp(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	if (nextToken != "{" || this->_contextLevel!= NONE || this->_http) {
		std::cerr << C_RED << "Unexcepted token after \"http\"" << RESET << std::endl;
		return;
	}
	this->_http = new Http();
	this->_contextConfig = this->_http;
	this->_contextLevel = HTTP;
}

void	ParseConfig::processServerToken(void) {
		std::string	nextToken;

		nextToken = getNextToken();
		if (nextToken != "{" || this->_contextLevel != HTTP || this->_currentConfig) {
			std::cerr << C_RED << "TEST" << RESET  << std::endl;
			return ;
		}
		this->_serverConfigs->push_back(new ServerConfig(*(this)->_http));
		this->_currentConfig = *(this)->_serverConfigs->rbegin();
		this->_contextConfig = this->_currentConfig;
		this->_contextLevel = SERVER;
}

void	ParseConfig::processLocationToken(void) {
	std::string	nextToken;

	nextToken = getNextToken();
	LOG(C_CORAL << "Inside location block" << RESET);
	if (nextToken != "{" || this->_contextLevel!= SERVER || this->_location) {
		std::cerr << C_RED << "Unexcepted token after \"http\"" << RESET << std::endl;
		return;
	}
	this->_location = new Location(*(this)->_currentConfig);
	this->_contextConfig = this->_location;
	this->_contextLevel = LOCATION;
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
	this->_contextConfig->setHeaderTimeout(timeout);
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
	this->_contextConfig->setBodyTimeout(timeout);
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
	this->_contextConfig->setMaxBodySize(maxBodySize);
}

/**
 * @detail root context: http, server, location
 */
void	ParseConfig::processRootToken(void) {
	std::string nextToken;

	nextToken = getNextToken();
	if (this->_contextLevel == HTTP){
		this->_http->setRoot(nextToken);
	}
	else if (this->_contextLevel == SERVER) {
		this->_currentConfig->setRoot(nextToken);
	}
	else if (this->_contextLevel == LOCATION) {
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
		if (this->_contextLevel != SERVER)
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
	if (this->_contextLevel == HTTP)
		this->_http->setAutoindex(is_enable);
	else if (this->_contextLevel == SERVER)
		this->_currentConfig->setAutoindex(is_enable);
	else if (this->_contextLevel == LOCATION)
		this->_location->setAutoindex(is_enable);
	else {
		LOG(C_RED << "Using token \"autoindex\" in wrong context" << RESET);
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
	if (this->_contextLevel == HTTP)
		this->_http->addErrorPage(code, pageToken);
	else if (this->_contextLevel == SERVER)
		this->_currentConfig->addErrorPage(code, pageToken);
	else if (this->_contextLevel == LOCATION)
		this->_location->addErrorPage(code, pageToken);
	else {
		LOG(C_RED << "Using token \"error_page\" in wrong context" << RESET);
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
	LOG(C_PURPLE << "DEBUG: number is: " << nb << RESET);
	return (nb);
}

