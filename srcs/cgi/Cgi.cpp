#include "../../include/Cgi.hpp"
#include "../../include/Server.hpp"
#include "../../include/Request.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/Client.hpp"

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

Cgi::Cgi() : _fileSystemPath(""), _tempEnv(), _finalEnv(), _stdinWriteEnd(-1), _stdoutReadEnd(-1)
{
}

Cgi::Cgi(Response *response, Request *request) : _fileSystemPath(""), _tempEnv(), _finalEnv(), _stdinWriteEnd(-1), _stdoutReadEnd(-1), _response(response), _request(request)
{
}

void Cgi::launch(const ServerConfig &config)
{
	errno = 0;

	this->_fileSystemPath = _response->getInternalPath();
	if (access(this->_fileSystemPath.c_str(), X_OK) == -1)
	{
		std::cerr << "access issue" << std::endl;
	}
	// std::string directoryPath;
	// size_t pos = this->_fileSystemPath.find_last_of('/');
	// directoryPath = this->_fileSystemPath.substr(0, pos);
	// std::string filename = this->_fileSystemPath.substr(pos + 1, this->_fileSystemPath.length());
	// if (chdir(directoryPath.c_str()) != 0)
	// 	std::cerr << "chdir error\n" << std::endl;
	// this->_fileName = filename;
	buildEnv(config);
	executeScript();
}

void Cgi::buildEnv(const ServerConfig &config)
{

	const std::map<std::string, std::string> &header = _request->getHeaders();
	std::string extension = this->_response->getCgiExtension();
	this->_scriptName = this->_fileSystemPath.substr(0, this->_fileSystemPath.find(extension) + extension.length());

	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("REDIRECT_STATUS=200");
	_tempEnv.push_back("REQUEST_METHOD=" + _request->getMethod());
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName);
	_tempEnv.push_back("PATH_INFO=" + this->_fileSystemPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself
	_tempEnv.push_back("QUERY_STRING=" + _request->getQuery());							   // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + _request->getVersion());
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	_tempEnv.push_back("SERVER_NAME=" + resolveHostname());
	std::cout << "content length sent to the cgi is " << _request->getContentLength() << std::endl;
	if (_request->getMethod() == "POST" && _request->getContentLength() != 0)
	{
		std::string contentType   = getHeaderVariable(header, "content-type");
	
		_tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length"));
		if (!contentType.empty())
			_tempEnv.push_back("CONTENT_TYPE=" + contentType);
	}

	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
	{
		this->_finalEnv.push_back(const_cast<char *>((*it).c_str()));
		std::cout << GREEN << *it << RESET << std::endl;
	}
	this->_finalEnv.push_back(NULL);
}

void	Cgi::openPipes(int *stdinPipe, int * stdoutPipe)
{
	if (pipe(stdoutPipe) == -1 || fcntl(stdoutPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		_response->setSyscallError();
	else if (pipe(stdinPipe) == -1 
		|| fcntl(stdinPipe[WRITE_END], F_SETFL, O_NONBLOCK) < 0
		|| fcntl(stdinPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		_response->setSyscallError();
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
		safeClose(stdinPipe[WRITE_END]);
		if (dup2(stdinPipe[READ_END], STDIN_FILENO) == -1)
		{
			safeClose(stdinPipe[READ_END]);
			safeClose(stdoutPipe[READ_END]);
			safeClose(stdoutPipe[WRITE_END]);
			std::exit(EXIT_FAILURE);
		}
		safeClose(stdinPipe[READ_END]);
		safeClose(stdoutPipe[READ_END]);
		if (dup2(stdoutPipe[WRITE_END], STDOUT_FILENO) == -1)
		{
			safeClose(stdoutPipe[WRITE_END]);
			std::exit(EXIT_FAILURE);
		}
		safeClose(stdoutPipe[WRITE_END]);
		std::string executable = this->_response->getCgiPass();

		char *argv[] = {const_cast<char *>(executable.c_str()), const_cast<char *>(this->_fileSystemPath.c_str()), NULL}; // NULL marks the end of the array
		if (execve(executable.c_str(), argv, this->_finalEnv.data()) == -1)
		{
			std::exit(EXIT_FAILURE); //WARN: What we do if execve() failed;
		}
	}
	safeClose(stdinPipe[READ_END]);
	if (_request->getMethod() == "POST")
	{
		_stdinWriteEnd = stdinPipe[WRITE_END];
		std::cout << "POST CASE the stdinwrite end is " << _stdinWriteEnd << std::endl;
	}
	else
		safeClose(stdinPipe[WRITE_END]);
	
	safeClose(stdoutPipe[WRITE_END]);
	_stdoutReadEnd = stdoutPipe[READ_END];
	this->_pid = pid;
}


int Cgi::getStdinWriteEnd() const
{
	return this->_stdinWriteEnd;
}

int Cgi::getPid(void) const
{
	return this->_pid;
}

int Cgi::getStdoutReadEnd() const
{
	return this->_stdoutReadEnd;
}

Cgi::~Cgi()
{
}
