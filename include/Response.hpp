#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfig.hpp"

#include <map>
#include <iostream>



class Response {
	private :
		int _statusCode;
		std::string _internalPath;
		std::string _reasonPhrase;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		Response();
		~Response();

		void build(Request& request, ServerConfig *config);
		std::string buildFileSystemPath(Location* loc, std::string& url, ServerConfig* config);


		void handleStaticRequest(Request& request, ServerConfig *config);
		void handleGetMethod(Request& request);
		void handlePostMethod(Request& request);
		void handleDeleteMethod(Request& request);
		void buildError();
};

#endif