#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>
#include "../include/color.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Location.hpp"

static void	openConfigFile(std::string &file, std::ifstream &fileStream);
static void	readParsingFile(std::ifstream &fileStream);

std::vector<ServerConfig> parseConfigFile(std::string configFile)
{
	std::ifstream				fileStream;
	std::vector<ServerConfig>	serversConfigs;
	
	try {
		openConfigFile(configFile, fileStream);
		readParsingFile(fileStream);
	}
	catch (std::invalid_argument &e) {
		std::cout << C_RED << e.what() << RESET << std::endl;
	}
	return (serversConfigs);
}

static void	openConfigFile(std::string &file, std::ifstream &fileStream)
{
	fileStream.open(file.c_str());
	if (!fileStream.is_open())
		throw std::invalid_argument("Unable to open the file " + file + " :" + strerror(errno));
}

static void	readParsingFile(std::ifstream &fileStream)
{
	std::string	line;
	std::string	buffer;

	while (getline(fileStream, buffer))
	{
		if (buffer == "")
			continue;
		std::cout << C_BLUE << buffer << std::endl;
	}
}

