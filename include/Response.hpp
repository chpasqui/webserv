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
		bool _isDir;

		//regarder pour les reasons phrases comment les integrer au mieux

	public:
		Response();
		~Response();

		void build(Request& request, ServerConfig *config, char **envp;
		Location* findLocation(const std::string& url, ServerConfig* config); // serait mieux dans serverConfig ou location
		std::string buildFileSystemPath(Location* loc, const std::string& url);
		bool methodAllowed(Location* loc, const std::string& method);
		bool isCGI(const std::string& path, Location* loc);

		void handleMethod(Request& request, Location* loc);

		void handleGetMethod(Request& request, Location* loc);
		void displayAutoIndex(); 

		void handlePostMethod(Request& request, Location* loc);

		void handleDeleteMethod(Request& request, Location* loc);
		void buildError();

		void handleCGI(Request& request, char **envp);

		void setSyscallError();
};

#endif