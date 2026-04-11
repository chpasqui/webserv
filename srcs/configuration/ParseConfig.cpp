#include "../../include/ParseConfig.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "../../include/color.hpp"
#include "../../include/ServerConfig.hpp"

static void	openConfigFile(std::string &file, std::ifstream &fileStream);
static void	readParsingFile(std::ifstream &fileStream);

ParseConfig::ParseConfig()
{
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename)
{
	std::ifstream				fileStream;
	std::vector<ServerConfig>	serversConfigs;
	
	fileStream.open(filename.c_str());
	if (!fileStream.is_open())
		throw std::invalid_argument("Unable to open the file " + filename + " :" + strerror(errno));
	
}




