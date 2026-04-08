#include "../include/Request.hpp"

void debugListHeader(std::map<std::string, std::string> &_headers){
std::map<std::string, std::string>::iterator it = _headers.begin();
	for(; it != _headers.end(); it++)
		std::cout << it->first << " value : " << it->second << std::endl;
}