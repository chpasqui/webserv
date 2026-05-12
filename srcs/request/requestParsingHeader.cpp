#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"

#include <vector>
#include <string>
#include <climits>
#include <algorithm>
#include <errno.h>

#define HEADERMAXSIZE 16000

static std::string extractHeaderSection(std::string request);

/**
 * @brief Parses all header fields and stores them in _headers map.
 * @param request The raw request buffer.
 * @return true on success, false on error with statusCode updated.
 */
bool Request::parseHeader(const std::string &request){ 

	std::string header = extractHeaderSection(request);
	if (header.empty())	
		return(setError(BAD_REQUEST, "parseHeader: empty section"));
	std::string line;
	std::istringstream split(header);

	while(std::getline(split, line)){
		if (!validateCRLF(line))
			return false;
		line.erase(line.end()-1);
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
	//LOG(debugListHeader(_headers));
	return true ;
}

/**
 * @brief Extracts the headers section from the raw request buffer.
 * @param request The raw request buffer.
 * @return Headers section as string, empty if "\r\n\r\n" not found.
 */
static std::string extractHeaderSection(std::string request){
	size_t start = request.find("\r\n") + 2;
	size_t end = request.find("\r\n\r\n");
	if (end == std::string::npos)
		return("");
	std::string header = request.substr(start, end - start + 2);
	return (header);
}

/**
 * @brief Checks line ends with '\r' (CRLF compliance).
 * @param line Current header line.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validateCRLF(const std::string &line){
	std::string::const_iterator it = line.end() - 1;
	if (*it != '\r'){
		return(setError(BAD_REQUEST, "validateCRLF: missing \\r"));
	}
	return true;
}

/**
 * @brief Rejects obsolete line folding (obs-fold) per RFC 7230.
 * @param line Current header line.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validateObsFold(const std::string &line){
	std::string::const_iterator it = line.begin();
	if (*it == ' ' || *it == '\t'){
		return(setError(BAD_REQUEST, "validateObsFold: obs-fold detected"));
	}
	return true ;
}

/**
 * @brief Validates field name format and character set.
 * @param line Current header line.
 * @param colonPos Position of ':' separator.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validateFieldName(const std::string &line, size_t colonPos){
	if (colonPos == std::string::npos || colonPos == 0 || line[colonPos - 1] == ' '){
		return(setError(BAD_REQUEST, "validateFieldName: colon missing/invalid"));
		}
	for(size_t i = 0; i < colonPos; i++){
		if (!isTokenChar(line[i]))
			return(setError(BAD_REQUEST, "validateFieldName: bad token char"));
	}
	return true ;
}

/**
 * @brief Extracts and lowercases the field name.
 * @param line Current header line.
 * @param colonPos Position of ':' separator.
 * @return Lowercased field name string.
 */
std::string Request::extractFieldName(const std::string &line, size_t colonPos){
	std::string fieldname = line.substr(0, colonPos);
	std::transform(fieldname.begin(), fieldname.end(), fieldname.begin(), ::tolower);
	return fieldname;
}


/**
 * @brief Extracts and left-trims the field value.
 * @param line Current header line.
 * @param colonPos Position of ':' separator.
 * @return Trimmed field value string.
 */
std::string Request::extractValue(const std::string &line, size_t colonPos){
		std::string value = line.substr(colonPos + 1, line.size());
		size_t trim = value.find_first_not_of(" \t");
		if (trim == std::string::npos)
			return ("") ;
		value = value.substr(trim);
		return value ;
}

/**
 * @brief Validates field value, with specific rules for content-length
 *        and transfer-encoding.
 * @param fieldname Lowercased field name.
 * @param value Trimmed field value.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validateFieldValue(const std::string &fieldname, const std::string &value){
	if (fieldname == "content-length"){
		if ((_headers.count("content-length") > 0) || (value.find(",") != std::string::npos))
			return(setError(BAD_REQUEST, "validateFieldValue: duplicate content-length"));
		errno = 0;
		long val = strtol(value.c_str(), NULL, 10);
		if (errno == ERANGE || val < 0 || val > INT_MAX)
			return(setError(BAD_REQUEST, "validateFieldValue: invalid content-length"));
		_contentLength = static_cast<int>(val);
	}
	if (fieldname == "transfer-encoding" && value != "chunked"){
		return(setError(BAD_REQUEST, "validateFieldValue: bad transfer-encoding"));
	}
	return true ;
}

/**
 * @brief Validates header set coherence (host, content-length vs
 *        transfer-encoding, POST requirements).
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validateHeader(){
	if (_headers.count("content-length") && _headers.count("transfer-encoding"))
		return(setError(BAD_REQUEST, "validateHeader: length/encoding conflict"));
	if (_method == "POST" && !_headers.count("transfer-encoding") && !_headers.count("content-length"))
		return(setError(LENGTH_REQUIRED, "validateHeader: POST no length"));
	if (_version == "HTTP/1.1" && !_headers.count("host"))
		return(setError(BAD_REQUEST, "validateHeader: missing host"));
	return true ;
}

