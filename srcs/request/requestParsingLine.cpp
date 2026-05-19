#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"
#include <vector>
#include <string>
#include <sstream>

/**
 * @brief Validates syntax, method, version and URL.
 * @param message The raw request buffer.
 * @return true on success, false on error with statusCode updated.
 */
bool Request::parseRequestLine(const std::string &message){
	
	if (!validSynthax(message))
		return false;
	if (!validMethod() || !validVersion() || !validUrl())
		return false; 
	return true;
}

/**
 * @brief Extracts method, URL and version from the request line.
 * @param message The raw request buffer.
 * @return true on success, false on error with statusCode updated.
 */
bool Request::validSynthax(const std::string &message){
	size_t end = message.find("\r\n");
    if (end == std::string::npos){
        return (setError(BAD_REQUEST, "validSynthax: missing CRLF"));
    }
	std::string line = message.substr(0, end);
	char splitChar = ' ';
	std::istringstream split(line);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, splitChar); tokens.push_back(each));
	if (tokens.size() != 3){
		return (setError(BAD_REQUEST, "validSynthax: invalid token count"));
	}
	_method = tokens[0];
	_url = tokens[1];
	_version = tokens[2];
	LOG_DEBUG("method='" << _method << "' url='" << _url << "' version='" << _version << "'");
	return true;
}

/**
 * @brief Checks method is GET, POST or DELETE.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validMethod(){

	if (_method != "GET" && _method != "POST" && _method != "DELETE"){
        return (setError(NOT_IMPLEMENTED, "validMethod: unknown method"));
    }
	return true;
}

/**
 * @brief Checks version is HTTP/1.0 or HTTP/1.1.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validVersion(){
	if (_version != "HTTP/1.0" && _version != "HTTP/1.1"){
        return (setError(WRONG_HTTP, "validVersion: unsupported version"));
    }
	return true ;
}

/**
 * @brief Checks URL format, length, unsafe chars and extracts query.
 * @return true if valid, false on error with statusCode updated.
 */
bool Request::validUrl(){
	if (_url.empty() || _url[0] != '/')
		return (setError(BAD_REQUEST, "validUrl: empty or missing /"));
	if (_url.size() > 2000)
		return (setError(URL_TOOLONG, "validUrl: URL too long"));
	if (_url.find(' ') != std::string::npos)
		return (setError(BAD_REQUEST, "validUrl: space in URL"));
	std::string url_decode = decodeUrl(_url);
	if (!checkUnsafeChar(url_decode))
		return (setError(BAD_REQUEST, "validUrl: unsafe characters"));

	stockIfQuery(url_decode);
    return true;
}

/**
 * @brief Splits URL into path and query string if '?' is present.
 * @param url_decode The decoded URL string.
 */
void	Request::stockIfQuery(std::string url_decode){

	size_t queryPos = url_decode.find('?');
	if (queryPos != std::string::npos){
		_query = url_decode.substr(queryPos + 1);
		_path = url_decode.substr(0, queryPos);
	}
	else
		_path = _url;
}

