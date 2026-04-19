#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"

std::string Response::responseFormat(Request& request){
	std::ostringstream response;

	setDefaultHeaders();

	std::map<std::string, std::string>::const_iterator it = _headers.begin();

	response 	<< request.getVersion() << " " << _statusCode << " " << _reasonPhrase <<"\n";
				for (; it != _headers.end(); it++){
					response << it->first << ": " << it->second << "\r\n";
				}
				response << "\r\n\r\n"
				<< _body << "\n";
}