#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <map>
#include <iostream>

class Response {
	int _statusCode;
	std::string _reasonPhrase;
	std::map<std::string, std::string> _headers;
	std::string _body;

};

#endif