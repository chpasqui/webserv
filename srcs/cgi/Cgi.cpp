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

Cgi::Cgi(const std::string& fileSystemPath) : _fileSystemPath(fileSystemPath), _tempEnv(), _finalEnv() 
{
	
}

int	Cgi::launch(const Request& request, const ServerConfig& config)
{
	if (access(this->_fileSystemPath.c_str(), X_OK) == -1) {
		std::cerr << "access issue" << std::endl;
		return -1;
	}
	buildEnv(request, config);
	int stdoutPipe = executeScript(request);
	return stdoutPipe;
}

void	Cgi::buildEnv(const Request& request, const ServerConfig& config) {
	
	const std::map<std::string, std::string>& header = request.getHeaders();
	this ->_scriptName =  this->_fileSystemPath.substr(0, this->_fileSystemPath.find(".py") + 3);

	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("REQUEST_METHOD=" + request.getMethod());
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName);
	_tempEnv.push_back("PATH_INFO=" + this->_fileSystemPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself
	_tempEnv.push_back("QUERY_STRING=" + request.getQuery()); // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + request.getVersion());
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	_tempEnv.push_back("SERVER_NAME=" + resolveHostname());

	if (request.getContentLength() != 0) {
		_tempEnv.push_back("CONTENT_TYPE=" + getHeaderVariable(header, "content-type"));
		 _tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length"));
	}

	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
		this->_finalEnv.push_back(const_cast<char *>((*it).c_str()));
	this->_finalEnv.push_back(NULL);
}

int	Cgi::executeScript(const Request& request)
{
	int stdoutPipe[2];
	int stdinPipe[2];
	std::string responseBody;

	if (pipe(stdoutPipe) == -1)
		perror("Error: ");
	
	if (fcntl(stdoutPipe[READ_END], F_SETFL, O_NONBLOCK) < 0)
		perror("Error: ");

	if (request.getMethod() == "POST") { 
		if (pipe(stdinPipe) == -1)
			perror("Error: ");
		if (fcntl(stdinPipe[WRITE_END], F_SETFL, O_NONBLOCK) < 0)
			perror ("Error: ");
	}

	pid_t pid = fork();	//
	if (pid == 0)
	{
		if (request.getMethod() == "POST") {
			close(stdinPipe[WRITE_END]);
			dup2(stdinPipe[READ_END], STDIN_FILENO);
			close(stdinPipe[READ_END]);
		}
		close(stdoutPipe[READ_END]);
		dup2(stdoutPipe[WRITE_END], STDOUT_FILENO);
		close(stdoutPipe[WRITE_END]); 
		std::string executable = parseShebang();

		char *argv[] = { const_cast<char *>(executable.c_str()), const_cast<char *>(this->_fileSystemPath.c_str()), NULL }; // NULL marks the end of the array
		if (execve(executable.c_str(), argv, this->_finalEnv.data()) == -1)
			std::exit(1);
	}
	else {
		if (request.getMethod() == "POST") {
			close(stdinPipe[READ_END]);
			write(stdinPipe[WRITE_END], request.getBody().c_str(), request.getBody().size());
			close(stdinPipe[WRITE_END]);		
		}
		int status;
		waitpid(pid, &status, 0);

		close(stdoutPipe[WRITE_END]);

		char buffer[1024];
		int contentLength = 0;
		do {
			contentLength = read(stdoutPipe[READ_END], buffer, 1024);
			if (contentLength > 0)
				responseBody.append(buffer, contentLength);
		} while(contentLength > 0);
		std::string contentLengthStr = "content-length=" + intToString(responseBody.size()) + '\n';
		responseBody.insert(0, contentLengthStr);
		std::cout << "DEBUG: responsebody from cgi (function that will be moved to server) is" << responseBody << std::endl;
	}
	return stdoutPipe[READ_END];
}

Cgi::~Cgi()
{

}
