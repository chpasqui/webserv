#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include <vector>
#include <string>


#include <climits>
#include <algorithm>

#define HEADERMAXSIZE 16000
// PARSING GENERIQUE DU HEADER
// Step1: définir début headers : buffer.find("\r\n") + 2 -- OK
// Step2: définir fin headers   : buffer.find("\r\n\r\n") -- OK
// Step3: extraire headersSection = buffer.substr(start, end - start) -- OK
// Step4: boucle getline chaque ligne -- OK
// Step 5: enlever \r final -- OK
// Step 6: si ligne vide lets break -> car plus rien -- OK
// Step 6.2 : verifier le Obs-fold qui est obsolete -- OK
// Step 7: trouver ":" if absent ou pos 0 ou espace avant --> error 400 -- OK
// Step 8: extraire fieldname -- OK
// Step 8.1 : verifier if valid caractere  -- OK
// Step 8.2 : toLower(avant ":") -- OK
// Step 9: vérifier caractères valides dans name → 400 si invalide -- TO CHECK THAT 
// Step 10: extraire value = trim(après ":") -- OK
// Step 11: if name == "content-length" : -- OK
//     - déjà dans map -> error 400 (doublon) -- OK
//     - if contient virgule -> erro 400 -- OK
//     - if pas un nombre -> error 400 -- OK
//     - check for overflow (> INT_MAX) sinon error 400 -- OK
// Step 12: stocker dans map header lalala -- OK

static std::string extractHeaderSection(std::string request);

bool Request::parseHeader(const std::string &request){ 

	std::string header = extractHeaderSection(request);
	std::string line;
	std::istringstream split(header);

	while(std::getline(split, line)){
		if (!validateCRLF(line))
			return false;
		line.pop_back();
		if (line.empty())
			break ;
		if(!validateObsFold(line))
			return false;

		size_t colonPos = line.find(':');
		if(!validateFieldName(line, colonPos))
			return false;

		std::string fieldname = extractFieldName(line, colonPos);
		std::string value = extractValue(line, colonPos);
		
		if (!validateFieldValue(fieldname, value))
			return false;
		
		_headers.insert(std::make_pair(fieldname, value));
	}
	debugListHeader(_headers);
	return true ;
}

static bool isTokenChar(unsigned char c){
	if(isalnum(c))
		return true;
	std::string special = "!#$%&'*+-.^_`|~";
	return (special.find(c) != std::string::npos);
}

static std::string extractHeaderSection(std::string request){
	size_t start = request.find("\r\n") + 2;
	size_t end = request.find("\r\n\r\n");
	std::string header = request.substr(start, end - start + 2);
	return (header);
}

bool Request::validateCRLF(const std::string &line){
	if (line.back() != '\r'){
		std::cout << line << std::endl;
		return(setError(BAD_REQUEST));
	}
	return true;
}

bool Request::validateObsFold(const std::string &line){
	if (line.front() == ' ' || line.front() == '\t'){
		return(setError(BAD_REQUEST));
	}
	return true ;
}

bool Request::validateFieldName(const std::string &line, size_t colonPos){
	if (colonPos == std::string::npos || colonPos == 0 || line[colonPos - 1] == ' '){
		return(setError(BAD_REQUEST));
		std::cout << "error colonPos" << std::endl;
		}
	for(size_t i = 0; i < colonPos; i++){
		if (!isTokenChar(line[i]))
			return(setError(BAD_REQUEST));
	}
	return true ;
}

std::string Request::extractFieldName(const std::string &line, size_t colonPos){
	std::string fieldname = line.substr(0, colonPos);
	std::transform(fieldname.begin(), fieldname.end(), fieldname.begin(), ::tolower);
	return fieldname;
}

std::string Request::extractValue(const std::string &line, size_t colonPos){
		std::string value = line.substr(colonPos + 1, line.size());
		size_t trim = value.find_first_not_of(" \t");
		if (trim == std::string::npos)
			return ("") ;
		value = value.substr(trim);
		std:: cout << value << std::endl;
		return value ;
}

bool Request::validateFieldValue(const std::string &fieldname, const std::string &value){
	if (fieldname == "content-length"){
		if ((_headers.count("content-length") > 0) || (value.find(",") != std::string::npos))
			return(setError(BAD_REQUEST));
		errno = 0;
		long val = strtol(value.c_str(), NULL, 10);
		if (errno == ERANGE || val < 0 || val > INT_MAX)
			return(setError(BAD_REQUEST));
		_contentLength = static_cast<int>(val);
	}
	if (fieldname == "transfer-encoding" && value != "chunked"){
		return(setError(BAD_REQUEST));
	}
	return true ;
}

bool Request::validateHeader(){
	if (_headers.count("content-length") && _headers.count("transfer-encoding"))
		return(setError(BAD_REQUEST));
	if (_method == "POST" && !_headers.count("transfer-encoding") && !_headers.count("content-length"))
		return(setError(LENGTH_REQUIRED));
	if (_headers.count("content-length") == 0 && _headers.count("transfer-encoding") == 0)
		_contentLength = 0;
	if (_version == "HTTP/1.1" && !_headers.count("host"))
		return(setError(BAD_REQUEST));
	return true ;
}

bool Request::setError(int code) {
    _statusCode = code;
    return false;
}


//VALIDER LA SEMANTIQUE DE LA REQUEST
// Step 1 : verify if both Transfer-Encoding ET Content-Length présents -> error 400
// Step 2: if HTTP/1.1 et "host" absent --> error 400
// Step 3: si method == POST et ni "content-length" ni "transfer-encoding" --> error 411
//Step 4: attention faudrait verifier si la taille du content length est < maxbodysize


//ATTENTION POUR BOUCLE EPOLL:



// if (STATE == PARSING_REQUEST_LINE)
// {
//    	parseRequestLine(buffer);
//     STATE = PARSING_HEADERS;
// }

// if (STATE == PARSING_HEADERS)
// {
//     if (request.isValid) // au lieu de statusCode==200 par exemple
//         parseHeaders(buffer);
// 	  // need to validation sémantique
//     if (statusCode == 200)
//         validateRequest(maxbodysize);
    
//     STATE = PARSING_BODY;
// }

// if (STATE == PARSING_BODY)
// {
//     if (statusCode == 200)
//         defineBody(buffer);
//
//     STATE = COMPLETE;
// }

// if (STATE == COMPLETE)
// {
//     response = buildResponse(request, serverConfig);
//     // et on switch to EPOLLOUT
// }