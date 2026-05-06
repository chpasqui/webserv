#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfig.hpp"
#include "Location.hpp"
#include "Session.hpp"

#include <map>
#include <string>
#include <sstream>

class Response {
	private :
		const ServerConfig* 				_config;
		Location* 							_loc;
		int 								_statusCode;
		std::string 						_internalPath;
		std::string 						_remain;
		std::map<std::string, std::string> 	_headers;
		std::string 						_body;

		Session*							_session;
		bool								_isNewSession;
		bool								_isCgi;

		std::string							_cgiPass;
		

		//route
		Location* 	findLocation(const std::string& url); // serait mieux dans serverConfig ou location
		std::string buildFileSystemPath(const std::string& url);
		bool 		methodAllowed(const std::string& method);
		bool 		isCGI();
		void 		handleMethod(Request& request);

		// handlers
		void handleGetMethod(Request& request);
		bool serveFile();
		void displayAutoIndex();
		void handlePostMethod(Request& request);
		void handleDeleteMethod(Request& request);

		// response building and formatting
		void 		buildError();
		void 		generateErrorPage();
		void 		setSyscallError();
		void 		setDefaultHeaders();
		std::string extractContentType();
		std::string getReasonPhrase() const;

		void setError(int code);
		
	public:
		Response();
		Response(Session& session, bool isNew);
		~Response();

		void 		build(Request& request, const ServerConfig *config);
		std::string responseFormat(Request& request);
		bool		getCgiStatus(void) const;
		std::string	getInternalPath(void) const;
		std::string	getCgiPass(void) const;
		void 		buildCgiResponse(const std::string& output);
		bool		splitOutput(const std::string& output, std::string& headers);
		void		parseCgiHeaders(std::string& headerSection);

		void serverSessionPage();
		void handleLogin(Request& request);
};

#endif 