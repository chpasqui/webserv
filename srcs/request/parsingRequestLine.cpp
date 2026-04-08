#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include <vector>
#include <string>
#include <sstream>


static std::string decodeUrl(std::string url);
static bool checkUnsafeChar(std::string url_decode);

Request::Request() : _contentLength(-1), _statusCode(0), _body("") {}

Request::Request(std::string requestMessage) : _requestMessage(requestMessage), _contentLength(-1), _statusCode(0), _body("") {}

Request::~Request(){}

/*PARSE REQUESTLINE*/
//Step 1: verifier synthax: Request-Line   = Method SP Request-URI SP HTTP-Version CRLF -- OK
//Step 2: verifier methode correcte -- OK
//Step 3: verifier version HTTP -- OK
//Step 4: Parse URL -- OK
bool Request::parseRequestLine(const std::string &message){
	
	if (!validSynthax(message))
		return false;
	if (!validMethod() || !validVersion() || !validUrl())
		return false; 
	std::cout << "method : " << _method << "\n" << 
				 "url : " << _url << "\n" << 
				 "version : " << _version << "\n" <<  
				 "path : " <<  _path << "\n" << 
				 "query: " << _query << "\n" << 
				 "statusCode : " << _statusCode << std::endl;
	return true;
}

bool Request::validSynthax(const std::string &message){

	size_t end = message.find("\r\n"); //pour le moment "\0" pour tester mais sinon ca doit etre "\r\n"
    if (end == std::string::npos){
		std::cout << "error find >> error : " << _statusCode << std::endl;
        return (setError(BAD_REQUEST));
    }
	std::string line = message.substr(0, end);
	std::cout << line << std::endl;
	char splitChar = ' ';
	std::istringstream split(line);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, splitChar); tokens.push_back(each));
	if (tokens.size() != 3){
		std::cout << "error tokens >> error : " << _statusCode << std::endl;
		return (setError(BAD_REQUEST));
	}
	_method = tokens[0];
	_url = tokens[1];
	_version = tokens[2];
	return true;
}

bool Request::validMethod(){

	if (_method != "GET" && _method != "POST" && _method != "DELETE"){
		std::cout << "error method >> error : " << _statusCode << std::endl;
        return (setError(NOT_IMPLEMENTED));
    }
	return true;
}

//Step 1 : check version, for CGI chunk request needs to have HTTP/1.1
bool Request::validVersion(){
	if (_version != "HTTP/1.0" && _version != "HTTP/1.1"){
		std::cout << "error version error : " << _statusCode << std::endl;
        return (setError(WRONG_HTTP));
    }
	return true ;
}

//Step 1 : check if it is not empty and if it starts with / -- OK
//Step 2 : decoder the URL pour les caracteres unsafe / -- OK
//Step 3 : checker les unsafe caracteres et pas oublier le /../ -- OK
//Step 4 : checker la longeur de l'URL -- OK
//Step 5 : verify query --> ce qu'il y a apres un ? -- OK
bool Request::validUrl(){

    if (_url.empty() || _url[0] != '/'){
		std::cout << "error url empty || doesnt start with / >> error : " << _statusCode << std::endl;
        return (setError(BAD_REQUEST));
    }
	if (_url.size() > 2000){
		std::cout << "error URL too long >> error :" << _statusCode << std::endl;
		return (setError(URL_TOOLONG));
	}
	if (_url.find(' ') != std::string::npos){
		std::cout << "error url space >> error :" << _statusCode << std::endl;
		return (setError(BAD_REQUEST));
	}
	std::string url_decode = decodeUrl(_url);
	if (!checkUnsafeChar(url_decode)){
		std::cout << "error unsafe after decoded >> error :" << _statusCode << std::endl;
		return (setError(BAD_REQUEST));
	}

	stockIfQuery(url_decode);
    return true;
}

// Step 1: Decode hex to ascii value -- OK
// Step 2: Verify if ascii is a control CTL (octets 0 - 31) and DEL (127) -- OK
static std::string decodeUrl(std::string url){

	std::string decoded;
	for (size_t i = 0; i < url.size(); i++){
		if (url[i] == '%' && i + 2 < url.size()){
			std::string hex = url.substr(i + 1, 2);
			long ascii = std::strtol(hex.c_str(), NULL, 16);
			//std::cout << ascii << std::endl;
			if ((ascii >= 0 && ascii < 32) || ascii == 127)
				return ("-1");
			decoded += static_cast<char>(ascii);
			i += 2;
		}
		else 
			decoded += url[i];
	}
	//std::cout << decoded << std::endl;
	return decoded;
}

static bool checkUnsafeChar(std::string url_decode){

	if (url_decode == "-1" || url_decode.find('%') != std::string::npos ||
		url_decode.find('#') != std::string::npos || url_decode.find('<') != std::string::npos || 
		url_decode.find('<') != std::string::npos || url_decode.find("/../") != std::string::npos)
			return false;
    return true;
}

void	Request::stockIfQuery(std::string url_decode){

	size_t queryPos = url_decode.find('?');
	if (queryPos != std::string::npos){
		_query = url_decode.substr(queryPos + 1);
		_path = url_decode.substr(0, queryPos);
	}
	else
		_path = _url;
}

int	Request::getStatusCode(void) const{
	return (_statusCode);
}

int	Request::getContentLength(void) const{
	return (_contentLength);
}


// int main(int argc, char *argv[]){

// 	Request data;

// 	if (argc != 2)
// 		return 1;
// 	data.parseRequestLine(argv[1]);
// 	data.parseHeader(argv[1]);
// 	data.parseBody(argv[1], 1024);
// 	return (0);
// }