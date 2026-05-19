#include "../../include/Cgi.hpp"
#include "../../include/Server.hpp"
#include "../../include/Request.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Client.hpp"
#include "../../include/Debug.hpp"
#include "../../include/utils.hpp"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 1024

Cgi::Cgi() : 
	_fileSystemPath(""), 
	_tempEnv(), 
	_finalEnv(), 
	_pipeWriteEnd(-1), 
	_pipeReadEnd(-1),
	_server(NULL),
	_response(NULL),
	_request(NULL) {}

Cgi::Cgi(Response *response, Request *request, Server *server) : 
	_fileSystemPath(""),
	_tempEnv(), _finalEnv(),
	_pipeWriteEnd(-1),
	_pipeReadEnd(-1),
	_server(server),
	_response(response),
	_request(request) {}

void Cgi::launch(const ServerConfig &config)
{
	this->_fileSystemPath = _response->getInternalPath();
	if (access(this->_fileSystemPath.c_str(), X_OK) == -1)
	{
		this->_response->setSyscallError("Launch: access failed");
		return ;
	}
	buildEnv(config);
	executeScript();
}

void Cgi::buildEnv(const ServerConfig &config)
{

	const std::map<std::string, std::string> &header = _request->getHeaders();
	std::string extension = this->_response->getCgiExtension();
	this->_scriptName = this->_fileSystemPath.substr(0, this->_fileSystemPath.find(extension) + extension.length());
	
	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("SCRIPT_FILENAME=" + _fileSystemPath);
	_tempEnv.push_back("REDIRECT_STATUS=200");
	_tempEnv.push_back("REQUEST_METHOD=" + _request->getMethod());
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName);
	_tempEnv.push_back("PATH_INFO=" + this->_fileSystemPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself
	_tempEnv.push_back("QUERY_STRING=" + _request->getQuery());							   // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + _request->getVersion());
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	_tempEnv.push_back("SERVER_NAME=" + resolveHostname());
	if (_request->getMethod() == "POST" && _request->getContentLength() != 0)
	{
		std::string contentType   = getHeaderVariable(header, "content-type");
	
		_tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length"));
		if (!contentType.empty())
			_tempEnv.push_back("CONTENT_TYPE=" + contentType);
	}

	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
	{
		this->_finalEnv.push_back((strdup((*it).c_str())));
		LOG_INFO(GREEN << *it);
	}
	this->_finalEnv.push_back(NULL);
}

void	Cgi::openPipes(int *stdinPipe, int * stdoutPipe)
{
	if (pipe(stdoutPipe) == -1 || fcntl(stdoutPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		_response->setSyscallError("openPipes: failed to open stdoutPipe");
	else if (pipe(stdinPipe) == -1 
		|| fcntl(stdinPipe[WRITE_END], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(stdinPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		_response->setSyscallError("openPipes: failed to open stdinPipe");
}

void Cgi::executeScript()
{
	int stdoutPipe[2];
	int stdinPipe[2];
	std::string responseBody;
	
	openPipes(stdinPipe, stdoutPipe);
	if (errno)
		return ;
	
	pid_t pid = fork(); //
	if (pid == 0)
	{
		std::string directoryPath;
		size_t pos = this->_fileSystemPath.find_last_of('/');
		directoryPath = this->_fileSystemPath.substr(0, pos);
		std::string filename = this->_fileSystemPath.substr(pos + 1, this->_fileSystemPath.length());
		if (chdir(directoryPath.c_str()) == -1)
		{
			_response->setSyscallError("executeScript: chdir failed in child");
			safeClose(stdinPipe[WRITE_END]);
			safeClose(stdinPipe[READ_END]);
			safeClose(stdoutPipe[READ_END]);
			safeClose(stdoutPipe[WRITE_END]);
			this->_server->~Server();
			std::exit(EXIT_FAILURE);
		}
		this->_fileName = filename;

		safeClose(stdinPipe[WRITE_END]);
		if (dup2(stdinPipe[READ_END], STDIN_FILENO) == -1)
		{
			safeClose(stdinPipe[READ_END]);
			safeClose(stdoutPipe[READ_END]);
			safeClose(stdoutPipe[WRITE_END]);
			this->_server->~Server();
			std::exit(EXIT_FAILURE);
		}
		safeClose(stdinPipe[READ_END]);
		safeClose(stdoutPipe[READ_END]);
		if (dup2(stdoutPipe[WRITE_END], STDOUT_FILENO) == -1)
		{
			safeClose(stdoutPipe[WRITE_END]);
			this->_server->~Server();
			std::exit(EXIT_FAILURE);
		}
		safeClose(stdoutPipe[WRITE_END]);
		std::string executable = this->_response->getCgiPass();

		char *argv[] = {const_cast<char *>(executable.c_str()), const_cast<char *>(this->_fileName.c_str()), NULL}; // NULL marks the end of the array
		if (execve(executable.c_str(), argv, this->_finalEnv.data()) == -1)
		{
			this->_response->setSyscallError("ExecuteScript: execve failed");
			std::exit(EXIT_FAILURE); //WARN: What we do if execve() failed;
		}
	}
	safeClose(stdinPipe[READ_END]);
	if (_request->getMethod() == "POST")
	{
		_pipeWriteEnd = stdinPipe[WRITE_END];
	}
	else
	{
		safeClose(stdinPipe[WRITE_END]);
	}
	
	safeClose(stdoutPipe[WRITE_END]);
	_pipeReadEnd = stdoutPipe[READ_END];
	this->_pid = pid;
}


int Cgi::getPipeWriteEnd() const
{
	return this->_pipeWriteEnd;
}

int Cgi::getPipeReadEnd() const
{
	return this->_pipeReadEnd;
}

int Cgi::getPid(void) const
{
	return this->_pid;
}

Response *Cgi::getResponse(void)
{
	return (this->_response);
}

Cgi::~Cgi()
{
	safeClose(this->_pipeWriteEnd);
	safeClose(this->_pipeReadEnd);
	for (size_t i = 0; i < _finalEnv.size(); ++i)
		free(_finalEnv[i]);
}

void Cgi::closePipeReadEnd()
{
	safeClose(this->_pipeReadEnd);
}

void Cgi::closePipeWriteEnd()
{
	safeClose(this->_pipeWriteEnd);
}
