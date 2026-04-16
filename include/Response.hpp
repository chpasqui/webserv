#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfig.hpp"
#include "Location.hpp"

#include <map>
#include <iostream>

class Response {
	private :
		ServerConfig* _config;
		Location* _loc;
		int _statusCode;
		std::string _internalPath;
		std::string _reasonPhrase;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _remain;

		//regarder pour les reasons phrases comment les integrer au mieux

	public:
		Response();
		~Response();

		void build(Request& request, ServerConfig *config, char **envp;
		Location* findLocation(const std::string& url); // serait mieux dans serverConfig ou location
		std::string buildFileSystemPath(const std::string& url);
		bool methodAllowed(const std::string& method);
		bool isCGI(const std::string& path);

		void handleMethod(Request& request);

		void handleGetMethod(Request& request);
		void displayAutoIndex(); 

		void handlePostMethod(Request& request);

		void handleDeleteMethod(Request& request);
		void buildError();
		void generateErrorPage();

		void handleCGI(Request& request, char **envp);

		void setSyscallError();

		
};

#endif