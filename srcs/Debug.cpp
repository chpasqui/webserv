#include "../include/Debug.hpp"

#include <iostream>

Debug::Debug() {
	return ;
}

Debug::~Debug() {
	return ;
}

void Debug::debugListHeader(std::map<std::string, std::string> &_headers){
	std::map<std::string, std::string>::iterator it = _headers.begin();
		for(; it != _headers.end(); it++)
			LOG_DEBUG(it->first << " value : " << it->second << std::endl);
}

