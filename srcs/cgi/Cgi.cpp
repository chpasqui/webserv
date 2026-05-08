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

void Cgi::launch(const std::string &internalPath, const Request &request, const ServerConfig &config)
{
	std::cout << "internal path received is " << internalPath << std::endl;
	this->_fileSystemPath = internalPath;
	if (access(this->_fileSystemPath.c_str(), X_OK) == -1)
	{
		std::cerr << "access issue" << std::endl;
	}
	std::cout << "no access issue anymore" << std::endl;
	buildEnv(request, config);
	executeScript(request);
}

void Cgi::buildEnv(const Request &request, const ServerConfig &config)
{

	const std::map<std::string, std::string> &header = request.getHeaders();
	this->_scriptName = this->_fileSystemPath.substr(0, this->_fileSystemPath.find(".py") + 3);

	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("REQUEST_METHOD=" + request.getMethod());
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName);
	_tempEnv.push_back("PATH_INFO=" + this->_fileSystemPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself
	_tempEnv.push_back("QUERY_STRING=" + request.getQuery());							   // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + request.getVersion());
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	_tempEnv.push_back("SERVER_NAME=" + resolveHostname());
	std::cout << "content length sent to the cgi is " << request.getContentLength() << std::endl;
	if (request.getMethod() == "POST" && request.getContentLength() != 0)
	{
		std::string contentType   = getHeaderVariable(header, "content-type");
	
		_tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length"));
		if (!contentType.empty())
			_tempEnv.push_back("CONTENT_TYPE=" + contentType);
	}

	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
		this->_finalEnv.push_back(const_cast<char *>((*it).c_str()));
	this->_finalEnv.push_back(NULL);
}

void	Cgi::openPipes(int *stdinPipe, int * stdoutPipe)
{
	if (pipe(stdoutPipe) == -1)
		perror("Error: ");
	if (pipe(stdinPipe) == -1)
		perror("Error: ");
	if (fcntl(stdoutPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		perror("Error: ");
	if (pipe(stdinPipe) == -1)
		perror("Error: ");
	if (fcntl(stdinPipe[WRITE_END], F_SETFL, O_NONBLOCK) < 0)
		perror("Error: ");
	if (fcntl(stdinPipe[READ_END], F_SETFL, O_NONBLOCK) < 0) // the read end is also used inside the script
		perror("Error: ");

}

void Cgi::executeScript(const Request &request)
{
	int stdoutPipe[2];
	int stdinPipe[2];
	std::string responseBody;
	
	openPipes(stdinPipe, stdoutPipe);
	
	pid_t pid = fork(); //
	if (pid == 0)
	{
		safeClose(stdinPipe[WRITE_END]);
		dup2(stdinPipe[READ_END], STDIN_FILENO);
		safeClose(stdinPipe[READ_END]);
		safeClose(stdoutPipe[READ_END]);
		dup2(stdoutPipe[WRITE_END], STDOUT_FILENO);
		safeClose(stdoutPipe[WRITE_END]);
		std::string executable = parseShebang(this->_fileSystemPath);

		char *argv[] = {const_cast<char *>(executable.c_str()), const_cast<char *>(this->_fileSystemPath.c_str()), NULL}; // NULL marks the end of the array
		if (execve(executable.c_str(), argv, this->_finalEnv.data()) == -1)
			std::exit(1);
	}
	safeClose(stdinPipe[READ_END]);
	if (request.getMethod() == "POST")
	{
		_stdinWriteEnd = stdinPipe[WRITE_END];
		std::cout << "POST CASE the stdinwrite end is " << _stdinWriteEnd << std::endl;
	}
	else
		safeClose(stdinPipe[WRITE_END]);
	
	safeClose(stdoutPipe[WRITE_END]);
	_stdoutReadEnd = stdoutPipe[READ_END];
	std::cout << "ALL CASES the _stdoutReadEnd end is " << _stdoutReadEnd << std::endl;
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
