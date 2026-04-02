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
	if (!parseUrl());
		return false;
	std::cout << _method << ' ' << _url << ' ' << _version << std::endl;
	return true;
}

bool Request::checkSpace(const std::string &message){
	size_t end = message.find("\r\n");
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
	if (_version != "HTTP/0.9" && _version != "HTTP/1.0") // a voir si on garde 0.9
    {
        _statusCode = 505;
		std::cout << "error version" << std::endl;
        return false;
    }
	return true ;
}

bool Request::parseUrl(){
    if (_url.empty() || _url[0] != '/'){
        _statusCode = 400;
        return false;
    }
    if (i dont know)

    return true;
}

int main(int argc, char *argv[]){
	Request data;
	if (argc != 2)
		return 1;
	data.parseRequestLine(argv[1]);
	return (0);
}

// PARSING GENERIQUE DU HEADER
bool Request::parseHeader(){ 
// Step1: définir début headers : buffer.find("\r\n") + 2
// Step2: définir fin headers   : buffer.find("\r\n\r\n")
// Step3: extraire headersSection = buffer.substr(begin, end - begin)

// Step4: boule getline chaque ligne
	// Step 5: enlever \r final
	// Step 6: si ligne vide lets break -> car plus rien
	// Step 7: trouver ":" if absent ou pos 0 ou espace avant --> error 400
	// Step 8: extraire name  = toLower(avant ":")
	// Step 9: vérifier caractères valides dans name → 400 si invalide
	// Step 10: extraire value = trim(après ":")
	// Step 11: if name == "content-length" :
	//     - déjà dans map -> error 400 (doublon)
	//     - if contient virgule -> erro 400
	//     - if pas un nombre -> error 400
	//     - check for overflow (> INT_MAX) sinon error 400
	// Step 12: stocker dans map header lalala
}

//attention faudrait verifier si la taille du content length est < 

//VALIDER LA SEMANTIQUE DE LA REQUEST
// Step 1 : verify if both Transfer-Encoding ET Content-Length présents -> error 400
// Step 2: if HTTP/1.1 et "host" absent --> error 400
// Step 3: si method == POST et ni "content-length" ni "transfer-encoding" --> error 411


// faire un isValid au lieu decrire partout == 200 bool isValid() const { return _statusCode == 200; }

//ATTENTION POUR BOUCLE EPOLL:
if (STATE == PARSING_REQUEST_LINE)
{
   	parseRequestLine(buffer);
    STATE = PARSING_HEADERS;
}

if (STATE == PARSING_HEADERS)
{
    if (request.isValid) // au lieu de statusCode==200 par exemple
        parseHeaders(buffer);
	  // need to validation sémantique
    if (statusCode == 200)
        validateRequest(maxbodysize);
    
    STATE = PARSING_BODY;
}

if (STATE == PARSING_BODY)
{
    if (statusCode == 200)
        defineBody(buffer);
    STATE = COMPLETE;
}

if (STATE == COMPLETE)
{
    response = buildResponse(request, serverConfig);
    // et on switch to EPOLLOUT
}