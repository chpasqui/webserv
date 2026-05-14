#include "../include/Debug.hpp"

#include "../include/color.hpp"

#include <iostream>

Debug::Debug() {
	return ;
}

Debug::~Debug() {
	return ;
}

void	Debug::log(const std::string &msg, int level) {
	switch (LOG)
	{
		case 1:
			if (level >= DEBUG)
				std::cout << C_PINK << msg << RESET << std::endl;
		case 2:
			if (level >= INFO)
				std::cout << C_BLUE << msg << RESET << std::endl;
		case 3:
			if (level >= WARN)
				std::cout << C_YELLOW << msg << RESET << std::endl;
		case 4:
			if (level == ERR)
				std::cout << C_RED << msg << RESET << std::endl;
			return ;
		default:
			return ;
	}
}

void Debug::debugListHeader(std::map<std::string, std::string> &_headers){
std::map<std::string, std::string>::iterator it = _headers.begin();
	for(; it != _headers.end(); it++)
		std::cout << it->first << " value : " << it->second << std::endl;
}

