#include "../include/Client.hpp"
#include "../include/Cgi.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unistd.h>
#include <iostream>

#define BUFFERSIZE 1024

Client::Client() : bodyoffset(0),
				   _clientFd(-1),
				   _cgiPipeFd(-1),
				   _writeCgiPipeEnd(-1),
				   _readCgiPipeEnd(-1),
				   _output(""),
				   _bytesRead(-1),
				   _buffer(""),
				   _parsingStatus(PARSING_REQUEST_LINE),
				   _config(NULL),
				   _request(NULL),
				   _response(NULL),
				   _cgi(NULL)
{
	time(&(this)->_timestamp);
}

Client::Client(int fd, const ServerConfig *config) : bodyoffset(0),
													_clientFd(fd),
													 _cgiPipeFd(-1),
													 _writeCgiPipeEnd(-1),
													 _readCgiPipeEnd(-1),
													 _output(""),
													 _bytesRead(-1),
													 _buffer(""),
													 _parsingStatus(PARSING_REQUEST_LINE),
													 _config(config),
													 _request(new Request()),
													 _response(NULL)
{
	time(&(this)->_timestamp);
}

Client::Client(const Client &other) : bodyoffset(other.bodyoffset),
									  _clientFd(other._clientFd),
									  _cgiPipeFd(other._cgiPipeFd),
									  _writeCgiPipeEnd(other._writeCgiPipeEnd),
									  _readCgiPipeEnd(other._readCgiPipeEnd),
									  _output(other._output),
									  _bytesRead(other._bytesRead),
									  _buffer(other._buffer),
									  _parsingStatus(other._parsingStatus),
									  _config(other._config),
									  _request(other._request),
									  _response(NULL)
{
}

bool Client::bodyRead(std::string &buffer)
{
	size_t pos = buffer.find("\r\n\r\n");
	buffer = buffer.substr(pos + 4);
	if (this->_request->getContentLength() == -1)
	{
		if (buffer.find("0\r\n\r\n") == std::string::npos)
			return false;
		return true;
	}
	if (buffer.size() < (size_t)_request->getContentLength())
		return false;
	return true;
}

void Client::parseRequest()
{
	char buffer[BUFFERSIZE];

	if (this->_parsingStatus == COMPLETE || this->_parsingStatus == CLOSEFD)
		return;
	std::cout << "fd is " <<  _clientFd << std::endl;
	this->_bytesRead = recv(_clientFd, buffer, sizeof(buffer), MSG_DONTWAIT);
	if (_bytesRead == 0 || _bytesRead == -1)
	{
		this->_parsingStatus = CLOSEFD;
		return;
	}
	_buffer += std::string(buffer, _bytesRead);
	if (this->_parsingStatus == PARSING_REQUEST_LINE && this->_buffer.find("\r\n") != std::string::npos)
	{
		if (!_request->parseRequestLine(_buffer))
		{
			this->_parsingStatus = COMPLETE;
			return;
		}
		this->_parsingStatus = PARSING_HEADERS;
	}
	if (this->_parsingStatus == PARSING_HEADERS && this->_buffer.find("\r\n\r\n") != std::string::npos)
	{
		if (!_request->parseHeader(_buffer))
		{
			this->_parsingStatus = COMPLETE;
			return;
		}
		if (_request->getContentLength() <= 0)
		{
			this->_parsingStatus = COMPLETE;
			return;
		}
		this->_parsingStatus = PARSING_BODY; // as soon as i pass to PARSEBODY this is where I need to parse my CGI
	}
	if (this->_parsingStatus == PARSING_BODY && bodyRead(_buffer))
	{
		if (!_request->parseBody(_buffer, BUFFERSIZE))
		{
			this->_parsingStatus = COMPLETE;
			return;
		}
		this->_parsingStatus = COMPLETE;
	}
	return;
}

int Client::getClientFd() const
{
	return (this->_clientFd);
}

time_t Client::getTimestamp(void) const
{
	return (this->_timestamp);
}

int Client::getParsingStatus() const
{
	return (this->_parsingStatus);
}

Request *Client::getRequest(void) const
{
	return (this->_request);
}

const ServerConfig *Client::getServerConfig(void) const
{
	return this->_config;
}

Response *Client::getResponse(void) const
{
	return (this->_response);
}

void Client::setResponse(Session &session, bool isNew)
{
	this->_response = new Response(session, isNew);
}

std::string Client::getResponseStr() const
{
	return _response->responseFormat(*_request);
}

void Client::setCgi()
{
	this->_cgi = new Cgi();
}

Cgi	*Client::getCgi(void) const
{
	return this->_cgi;
}

int Client::getCgiPipeFd() const
{
	return (this->_cgiPipeFd);
}

void Client::setCgiPipeFd(int fd)
{
	this->_cgiPipeFd = fd;
}

void	Client::setOutput(const std::string& newOutput)
{
	this->_output = newOutput;
}

std::string	Client::getOutput(void) const
{
	return this->_output;
}

/**
 * @brief Destroy the Client:: Client object
 * Client is the one who holds the *Request, it must delete it then.
 *
 */
Client::~Client()
{
	if (this->_request)
		delete this->_request;
	if (this->_response)
		delete this->_response;
	if (this->_cgi)
		delete this->_cgi;
}
