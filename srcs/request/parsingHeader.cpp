#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#define HEADERMAXSIZE 16000
// PARSING GENERIQUE DU HEADER
// Step1: définir début headers : buffer.find("\r\n") + 2 -- OK
// Step2: définir fin headers   : buffer.find("\r\n\r\n") -- OK
// Step3: extraire headersSection = buffer.substr(start, end - start) -- OK
// Step4: boucle getline chaque ligne
bool Request::parseHeader(const std::string &request){ 

	size_t start = request.find("\r\n") + 2;
	size_t end = request.find("\r\n\r\n");
	std::string header = request.substr(start, end - start);
	if (header.size() > HEADERMAXSIZE) //a mettre cette valeur dans le fichier config
	while(getline(ss, line)){

	}




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