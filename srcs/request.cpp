#include "../include/Request.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


Request::Request() : _contentLength(0), _statusCode(200){}

Request::Request(std::string requestMessage) : _requestMessage(requestMessage){}

Request::~Request(){}


/*PARSE REQUESTLINE*/
//Etape 1 verifier space: Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
//Etape 2 verifier methode correcte
//Etape 3 verifier version HTTP

bool Request::parseRequestLine(const std::string &message){
	
	if (!checkSpace(message) || !checkMethod() || !checkVersion())
		return false;
	std::cout << _method << ' ' << _url << ' ' << _version << std::endl;
	return true;
}

bool Request::checkSpace(const std::string &message){
	size_t end = message.find("\0");
    if (end == std::string::npos){
        _statusCode = 400;
		std::cout << "error find" << std::endl;
        return false;
    }
	std::string line = message.substr(0, end);
	char splitChar = ' ';
	std::istringstream split(message);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, splitChar); tokens.push_back(each));
	if (tokens.size() != 3){
		_statusCode = 400;
		std::cout << "error tokens" << std::endl;
		return false;
	}
	_method = tokens[0];
	_url = tokens[1];
	_version = tokens[2];
	return true;
}

bool Request::checkMethod(){
	if (_method != "GET" && _method != "POST" && _method != "DELETE"){
        _statusCode = 501;
		std::cout << "error method" << std::endl;
        return false;
    }
	return true;
}

bool Request::checkVersion(){
	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
    {
        _statusCode = 505;
		std::cout << "error version" << std::endl;
        return false;
    }
	return true ;
}

int main(int argc, char *argv[]){
	Request data;
	if (argc != 2)
		return 1;
	data.parseRequestLine(argv[1]);
	return (0);
}

//bool Request::parseHeader(){}

