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
#include "../../include/HTTP.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Location.hpp"
#include "../../include/color.hpp"

ParseConfig::ParseConfig() : _context(0) {
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename) : _filename(filename), _context(0){
	std::cout << "ParseConfig: Normal construtor called" << std::endl;
}

ParseConfig::~ParseConfig() {
	std::cout << "ParseConfig: Destructor called" << std::endl;
}

int	ParseConfig::parsingConfigFile(std::vector<ServerConfig*> &serversConfigs) {
	ServerConfig *serverConfig = 0;
	this->openFile();
	while (processToken(*serverConfig) == 0)
		;
	serversConfigs.push_back(serverConfig);
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
	//ssize_t						line;

	pos = this->_configFile.find_first_not_of(" \n");
	this->_configFile.erase(this->_configFile.begin(), this->_configFile.begin() + pos);
	pos = this->_configFile.find_first_of(" \n");	//WARN: no protect for end, use find() to check the npos
	std::cout << pos << std::endl;
	token = this->_configFile.substr(0, pos);
	this->_configFile.erase(this->_configFile.begin(), this->_configFile.begin() + pos);

	std::cout << C_PINK << token << RESET << std::endl;
	return (token);
}

int	ParseConfig::processToken(ServerConfig &serverConfig) {
	std::string currentToken;
	std::string nextToken;

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
	
	nextToken = getNextToken();
	if (currentToken == "http") {
		std::cout << C_CORAL << "Inside HTTP block" << RESET << std::endl;
		if (nextToken != "{" || this->_context!= NONE) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		Http http;
		this->_context = HTTP;
		//TODO::	Create Server Class (insie a vector)
	}

	if (currentToken == "server") {
		std::cout << C_CORAL << "Inside server block" << RESET << std::endl;
		if (nextToken != "{" || this->_context != HTTP) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		this->_context = SERVER;
		//TODO::	Create Server Class (insie a vector)
	}

	else if (currentToken == "location") {
		std::cout << C_CORAL << "Inside location block" << RESET << std::endl;
		if (nextToken != "{" || this->_context!= SERVER) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		this->_context = LOCATION;
		//TODO: Create Location class
	}

	else if (currentToken == "root") {
		if (access(nextToken.c_str(), F_OK) == -1)
		{
			std::cerr << C_RED << "Access() failed: " << strerror(errno) << RESET << std::endl;
			return (1);
		}
		serverConfig.setRoute(nextToken);
	}

	else if (currentToken == "listen") {
		int 	port;
		
		if (this->_context != SERVER)
		{
			std::cerr << C_RED << "listen can only be called in a server context" << RESET << std::endl;
			return (1);
		}
		std::stringstream	ss(nextToken);
		ss >> port;
		if (ss.fail() || !ss.eof())
		{
			std::cerr << C_RED << "listen need only nuber" << RESET << std::endl;
			return (1);
		}
		serverConfig.setPort(port);
	}
	
	else {
		std::cerr << C_RED << currentToken << ": Unknow token" << RESET << std::endl;
		return (1);
	}
	
	return (0);
}

