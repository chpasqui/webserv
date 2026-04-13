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
#include "../../include/ServerConfig.hpp"
#include "../../include/color.hpp"

ParseConfig::ParseConfig() :_level(0) {
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename) : _filename(filename), _level(0){
	std::cout << "ParseConfig: Normal construtor called" << std::endl;
}

ParseConfig::~ParseConfig() {
	std::cout << "ParseConfig: Destructor called" << std::endl;
}

int	ParseConfig::parsingConfigFile(std::vector<ServerConfig*> &serversConfigs) {
	ServerConfig *serverConfig = new ServerConfig();
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
		this->_level -= 1;
		return (0);
	}
	
	nextToken = getNextToken();
	std::cout << C_CORAL << "currentToken is \"" << currentToken << "\" nextToken is \"" << nextToken << "\"" << RESET << std::endl;
	if (currentToken == "http") {
		std::cout << C_CORAL << "Inside HTTP block" << RESET << std::endl;
		if (nextToken != "{" || this->_level != NONE) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		this->_level = HTTP;
		//TODO: Create Http Class
	}

	else if (currentToken == "server") {
		std::cout << C_CORAL << "Inside server block" << RESET << std::endl;
		if (nextToken != "{" || this->_level != HTTP) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		this->_level = SERVER;
		//TODO::	Create Server Class (insie a vector)
		//			Create a default Location (inise a another vector)
	}

	else if (currentToken == "location") {
		std::cout << C_CORAL << "Inside location block" << RESET << std::endl;
		if (nextToken != "{" || this->_level != SERVER) {std::cerr << C_RED << "TEST" << RESET << std::endl; return (1);}
		this->_level = LOCATION;
		//TODO: Create Location class
	}

	else if (currentToken == "root") {
		if (access(nextToken.c_str(), F_OK) == -1)
		{
			std::cerr << C_RED << "Access() failed: " << strerror(errno) << RESET << std::endl;
			return (1);
		}
		serverConfig.setRoute(nextToken);
		//TODO:: Add root to ServerConfig Class
	}

	else if (currentToken == "listen") {
		int 	port;
		
		std::cout << C_MAGENTA << this->_level << RESET << std::endl;
		if (this->_level != SERVER)
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
		//TODO: Add "Port" to ServerConfig Class
	}
	
	else {
		std::cerr << C_RED << "Unknow token" << RESET << std::endl;
		return (1);
	}

	
	return (0);
}

