#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/Request.hpp"
#include "../include/Response.hpp"
#include "../include/Session.hpp"
// #include "../include/Cgi.hpp"
#include <ctime>
#include <string>
#include <sys/types.h>

class ServerConfig;
class Cgi;

enum ParsingStatus
{
	PARSING_REQUEST_LINE,
	PARSING_HEADERS,
	PARSING_BODY,
	COMPLETE,
	CLOSEFD
};

class Client
{
	public:
		Client(int fd, const ServerConfig *config);
		Client(const Client &other);
		~Client();

		void parseRequest();
		bool bodyRead(std::string &buffer);
	
		int getClientFd() const;
		time_t getTimestamp(void) const;
		int getParsingStatus() const;
		Request *getRequest(void) const;
		const ServerConfig *getServerConfig(void) const;
		Response *getResponse(void) const;
		void setResponse(Session &session, bool isNew);
		std::string getResponseStr() const;
		void setCgi();
		Cgi	*getCgi(void) const;
		int getCgiPipeFd(void) const;
		void setCgiPipeFd(int fd);

	private:
		Client(); // client constructor is in private so that we never create a client out of nowhere
		int _clientFd;
		int _cgiPipeFd;
		int _writeCgiPipeEnd;
		int _readCgiPipeEnd;

		ssize_t _bytesRead;
		std::string _buffer;
		id_t _parsingStatus;

		time_t _timestamp;

		const ServerConfig *_config;
		Request *_request;
		Response *_response;
		Cgi *_cgi;
};

#endif
