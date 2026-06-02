#include "../include/Debug.hpp"

#include <iostream>

Debug::Debug() {
	LOG_DEBUG(C_CSTR << "Debug: Default constructor called");
	return ;
}

Debug::~Debug() {
	LOG_DEBUG(C_DSTR << "Debug: Destructor called");
	return ;
}

void Debug::debugListHeader(std::map<std::string, std::string> &_headers){
	std::map<std::string, std::string>::iterator it = _headers.begin();
		for(; it != _headers.end(); it++)
			LOG_DEBUG(it->first << " value : " << it->second);
}

