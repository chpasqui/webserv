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
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _remain;

	public:
		Response();
		~Response();

		std::string getReasonPhrase() const;
		void setDefaultHeaders();

		void build(Request& request, ServerConfig *config, char **envp);
		Location* findLocation(const std::string& url); // serait mieux dans serverConfig ou location
		std::string buildFileSystemPath(const std::string& url);
		bool methodAllowed(const std::string& method);
		bool isCGI();

		void handleMethod(Request& request);

		void handleGetMethod(Request& request);
		bool serveFile();
		void displayAutoIndex(); 

		void handlePostMethod(Request& request);

		void handleDeleteMethod(Request& request);
		void buildError();
		void generateErrorPage();

		void handleCGI(Request& request, char **envp);

		void setSyscallError();

		std::string extractContentType();
		std::string responseFormat(Request& request);

		
};

#endif