#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

static std::string decodeUrl(std::string url);
static bool checkUnsafeChar(std::string url_decode);

Request::Request() : _contentLength(0), _statusCode(OK){}

Request::Request(std::string requestMessage) : _requestMessage(requestMessage), _contentLength(0), _statusCode(OK) {}

Request::~Request(){}

/*PARSE REQUESTLINE*/
//Etape 1 verifier space: Request-Line   = Method SP Request-URI SP HTTP-Version CRLF -- OK
//Etape 2 verifier methode correcte -- OK
//Etape 3 verifier version HTTP -- OK

bool Request::parseRequestLine(const std::string &message){
	
	if (!checkSpace(message) || !checkMethod() || !checkVersion())
		return false;
	if (!parseUrl())
		return false; 
	std::cout << _method << ' ' <<
				 _url << ' ' << 
				 _version << ' ' << 
				 _path << ' ' <<
				 _queryString << ' ' << std::endl;
	return true;
}

bool Request::checkSpace(const std::string &message){
	size_t end = message.find("\0");
    if (end == std::string::npos){
        _statusCode = BAD_REQUEST;
		std::cout << "error find" << std::endl;
        return false;
    }
	std::string line = message.substr(0, end);
	char splitChar = ' ';
	std::istringstream split(message);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, splitChar); tokens.push_back(each));
	if (tokens.size() != 3){
		_statusCode = BAD_REQUEST;
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
        _statusCode = NOT_IMPLEMENTED;
		std::cout << "error method" << std::endl;
        return false;
    }
	return true;
}

//Step1 : check version, for CGI chunk request needs to have HTTP/1.1
bool Request::checkVersion(){
	if (_version != "HTTP/1.0" && _version != "HTTP/1.1") 
    {
        _statusCode = WRONG_HTTP;
		std::cout << "error version" << std::endl;
        return false;
    }
	return true ;
}

//Step1 : check if it is not empty and if it starts with / -- OK
//Step2 : decoder the URL pour les caracteres unsafe / -- OK
//Step3 : checker les unsafe caracteres et pas oublier le /../ -- OK
//Step4 : checker la longeur de l'URL -- OK

// Do we have to verify the relative path 
// what about netpath ?
// for relative path :
//// verify param --> ce qu il y a apres un semi colon ?
//// verify query --> ce qu'il y a apres un ? -- OK

bool Request::parseUrl(){
	std::string url_decode;

    if (_url.empty() || _url[0] != '/'){
        _statusCode = BAD_REQUEST;
		std::cout << "error url empty || doesnt start with /" << std::endl;
        return false;
    }
	if (_url.size() > 2000){
		_statusCode = URL_TOOLONG;
		std::cout << "error URL too long" << std::endl;
		return false;
	}
	if (url_decode.find(' ') != std::string::npos){
		_statusCode = BAD_REQUEST;
			std::cout << "error url unsafe caractere" << std::endl;
			return false;
	}
	url_decode = decodeUrl(_url);
	if (!checkUnsafeChar(url_decode)){
		_statusCode = BAD_REQUEST;
		std::cout << "error unsafe after decoded" << std::endl;
		return false;
	}
	stockIfQuery(url_decode);
    return true;
}

void	Request::stockIfQuery(std::string url_decode){
	size_t queryPos = url_decode.find('?');
	if (queryPos != std::string::npos){
		_queryString = url_decode.substr(queryPos + 1);
		_path = url_decode.substr(0, queryPos);
	}
}

static bool checkUnsafeChar(std::string url_decode){
	if (url_decode == "-1" || url_decode.find('%') != std::string::npos || url_decode.find('#') != std::string::npos ||
		url_decode.find('<') != std::string::npos || url_decode.find('<') != std::string::npos ||
		url_decode.find("/../") != std::string::npos){
			return false;
	}
    return true;
}

// Step1: Decode -- OK
// Step2: verify if ascii is a control CTL (octets 0 - 31) and DEL (127) -- OK
static std::string decodeUrl(std::string url){
	std::string decoded;
	for (size_t i = 0; i < url.size(); i++){
		if (url[i] == '%' && i + 2 < url.size()){
			std::string hex = url.substr(i + 1, 2);
			long ascii = std::strtol(hex.c_str(), NULL, 16);
			std::cout << ascii << std::endl;
			if ((ascii >= 0 && ascii < 32) || ascii == 127)
				return ("-1");
			decoded += static_cast<char>(ascii);
			i += 2;
		}
		else 
			decoded += url[i];
	}
	std::cout << decoded << std::endl;
	return decoded;
}

int main(int argc, char *argv[]){
	Request data;
	if (argc != 2)
		return 1;
	data.parseRequestLine(argv[1]);
	return (0);
}

