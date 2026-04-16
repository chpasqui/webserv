#include "../../include/ParseConfig.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
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
		std::cout << "token } find" << std::endl;
		this->_context -= 1;
		return (0);
		//TODO: Send "Location" or ServerConfig" classes to respective Vector
	}
	
	else if (currentToken == "http")
		this->processTokenHttp();

	else if (currentToken == "server") {
		this->processServerToken(serverConfigs);
	}

	else if (currentToken == "location") {
		this->processLocationToken();
	}

	else if (currentToken == "root") {
		this->processRootToken();
	}

	else if (currentToken == "listen") {
		this->processListenToken();
	}

	else {
		std::cerr << C_RED << currentToken << ": Unknow token" << RESET << std::endl;
		return (1);
	}
	
	return (0);
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
		std::cout << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
	}
	this->_context = HTTP;
	return (0);
}

int ParseConfig::processServerToken(std::vector<ServerConfig *> servers) {
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
			std::cout << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
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
		std::cout << C_RED << "Fatal error: " << e.what() << RESET << std::endl;
		return (1);
	}
	this->_context = LOCATION;
	return (0);
}

int ParseConfig::processRootToken(void) {
	std::string nextToken;

	nextToken = getNextToken();
	if (this->_context == SERVER) {
		this->_currentServer->setRoute(nextToken);
	}
	else if (this->_context == LOCATION) {
		this->_location->setRoot(nextToken);
	}
	else {
		std::cout << C_RED << "Using token \"root\" in wrong context" << RESET << std::endl;
		return (1);
	}
	return (0);
}

int ParseConfig::processListenToken(void) {
		int 		port;
		std::string	nextToken;

		nextToken = getNextToken();
		if (this->_context != SERVER)
		{
			std::cout << C_RED << "Using token \"listen\" in wrong context" << RESET << std::endl;
			return (1);
		}
		std::stringstream	ss(nextToken);
		ss >> port;
		if (ss.fail() || !ss.eof())
		{
			std::cerr << C_RED << "listen need only nuber" << RESET << std::endl;
			return (1);
		}
		this->_currentServer->setPort(port);
		return (1);
}

int ParseConfig::processErrorPageToken(void) {
	
}
