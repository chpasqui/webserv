#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfig.hpp"
#include "Location.hpp"

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

		//responseBuild.cpp
		void build(Request& request, ServerConfig *config);
		Location* findLocation(const std::string& url, ServerConfig* config); // serait mieux dans serverConfig ou location
		std::string buildFileSystemPath(Location* loc, const std::string& url, ServerConfig* config);
		bool methodAllowed(Location* loc, const std::string& method);


		void handleMethod(Request& request, ServerConfig *config);
		void handleGetMethod(Request& request);
		void handlePostMethod(Request& request);
		void handleDeleteMethod(Request& request);
		void buildError();
		void handleCGI();
};

#endif