#include "../../include/ParseConfig.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "../../include/color.hpp"

static void	openConfigFile(std::string &file, std::ifstream &fileStream);
static void	readParsingFile(std::ifstream &fileStream);

ParseConfig::ParseConfig()
{
	std::cout << "ParseConfig: Default constructor called" << std::endl;
}

ParseConfig::ParseConfig(std::string filename)
{
	std::cout << "ParseConfig: Normal construtor called" << std::endl;
}

int		ParseConfig::openFile(std::string &filename)
{
	std::ifstream	fileStream;

	fileStream.open(filename.c_str());
	if (!fileStream.is_open())
	{
		std::cerr << C_RED << "Unable to open the file " << filename << " :" << strerror(errno) << RESET << std::endl;
		return (errno);
	}
	fileStream >> this->_configFile;

}




