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
	int stdoutPipe[2];	// the stdoutPipe file descriptor pair inherited by child
	int stdinPipe[2];	// the stdin file descriptor pair inherited by child
	std::string responseBody;

	if (pipe(stdoutPipe) == -1)
		perror("Error: ");
	
	// setting the read end of the pipe non blocking
	if (fcntl(stdoutPipe[READ_END], F_SETFL, O_NONBLOCK) < 0) // if a process atempts to read from an empty pipe it can block and writing to a full pipe will also block. 
		perror("Error: ");

	// Handling when it is POST
	if (request.getMethod() == "POST") { 
		//std::cout << "the request.getMethod() is post, so in this case, we create the std:::in pipe" << std::endl;
		if (pipe(stdinPipe) == -1)
			perror("Error: ");
		if (fcntl(stdinPipe[WRITE_END], F_SETFL, O_NONBLOCK) < 0) // set the write end to ONONBLOCK because normally the write() call would wait for the pipe to be read to add new data. it wil instead write what it can, and then immdiately return
			perror ("Error: ");
	}

	pid_t pid = fork();	//
	// error case
	if (pid == 0)
	{
		if (request.getMethod() == "POST") {
			//std::cout << "entered the post pipe creation" << std::endl;
			close(stdinPipe[WRITE_END]); // the child will readn from the pipe
			dup2(stdinPipe[READ_END], STDIN_FILENO); // the STDIN of the child inc ase of POST is the pipe read end
			close(stdinPipe[READ_END]);	// can close the original fd
		}
		//std::cout << "inside the child after the POST method" << std::endl;
		close(stdoutPipe[READ_END]);
		dup2(stdoutPipe[WRITE_END], STDOUT_FILENO); // when the process will output something, it will go to the read end of the stdoutPipe
		close(stdoutPipe[WRITE_END]); 
		std::string executable = parseShebang();
		//std::cout << executable << internalPath << std::endl;
		char *argv[] = { const_cast<char *>(executable.c_str()), const_cast<char *>(this->_fileSystemPath.c_str()), NULL }; // NULL marks the end of the array
		if (execve(executable.c_str(), argv, this->_finalEnv.data()) == -1)
			std::exit(1);
	}
	else {
		if (request.getMethod() == "POST") {
			//std::cout << "entered the parent POST" << std::endl;
			close(stdinPipe[READ_END]);
			write(stdinPipe[WRITE_END], request.getBody().c_str(), request.getBody().size());
			close(stdinPipe[WRITE_END]);		
		}
		int status;
		waitpid(pid, &status, 0);

		close(stdoutPipe[WRITE_END]);

		// FOR NOW DEBUG PURPOSES
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
