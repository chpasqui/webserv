#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <climits>

#define HEADERMAXSIZE 16000
// PARSING GENERIQUE DU HEADER
// Step1: définir début headers : buffer.find("\r\n") + 2 -- OK
// Step2: définir fin headers   : buffer.find("\r\n\r\n") -- OK
// Step3: extraire headersSection = buffer.substr(start, end - start) -- OK
// Step4: boucle getline chaque ligne -- OK
// Step 5: enlever \r final -- OK
// Step 6: si ligne vide lets break -> car plus rien -- OK
// Step 7: trouver ":" if absent ou pos 0 ou espace avant --> error 400 -- OK
// Step 8: extraire name  = toLower(avant ":") -- OK
// Step 9: vérifier caractères valides dans name → 400 si invalide -- TO CHECK THAT 
// Step 10: extraire value = trim(après ":") -- OK
// Step 11: if name == "content-length" : -- OK
//     - déjà dans map -> error 400 (doublon) -- OK
//     - if contient virgule -> erro 400 -- OK
//     - if pas un nombre -> error 400 -- OK
//     - check for overflow (> INT_MAX) sinon error 400 -- OK
// Step 12: stocker dans map header lalala -- OK
bool Request::parseHeader(const std::string &request){ 

	size_t start = request.find("\r\n") + 2;
	size_t end = request.find("\r\n\r\n");
	std::string header = request.substr(start, end - start);
	std::string line;
	std::istringstream split(header);
	while(std::getline(split, line)){
		if (line.find('\r') == std::string::npos && line[line.size() - 1] != '\r'){
			_statusCode = BAD_REQUEST;
			return false;
		}
		line.resize(line.size() - 1);
		if (line.empty())
			break ;
	}
	size_t colonPos = request.find(':');
	if (colonPos == std::string::npos || colonPos == 0 || request[colonPos - 1] == ' '){
		_statusCode = BAD_REQUEST;
		return false;
	}
	std::string fieldname = request.substr(0, colonPos);
	for (size_t i = 0; i < fieldname.size(); i++){
		std::tolower(fieldname[i]);
	}
	std::string value = request.substr(colonPos + 1, line.size());
	if (fieldname == "content-length"){
		if ((_headers.count("content-lenght") > 0) || (value.find(",") != std::string::npos))
			_statusCode = BAD_REQUEST;
		if (strtol(value.c_str(), NULL, 10) < 0 || strtol(value.c_str(), NULL, 10) > INT_MAX)
			_statusCode = BAD_REQUEST;
		if (errno == ERANGE)
			_statusCode = BAD_REQUEST;
		if (!isValid())
			return false;
	}
	_headers.insert(std::make_pair(fieldname, value));
}

bool Request::isValid() const { return _statusCode == 200; }

//VALIDER LA SEMANTIQUE DE LA REQUEST
// Step 1 : verify if both Transfer-Encoding ET Content-Length présents -> error 400
// Step 2: if HTTP/1.1 et "host" absent --> error 400
// Step 3: si method == POST et ni "content-length" ni "transfer-encoding" --> error 411
//Step 4: attention faudrait verifier si la taille du content length est < maxbodysize


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