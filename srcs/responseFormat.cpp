#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"

/**
 * @brief Serializes the response to a raw HTTP/1.1 string.
 */
std::string Response::responseFormat(Request& request){
	std::ostringstream response;

	setDefaultHeaders();

	std::map<std::string, std::string>::const_iterator it = _headers.begin();

	response 	<< request.getVersion() << " " << _statusCode << " " << getReasonPhrase() <<"\r\n";
				for (; it != _headers.end(); it++){
					response << it->first << ": " << it->second << "\r\n";
				}
				response << "\r\n"
				<< _body << "\n";
	return response.str();
}