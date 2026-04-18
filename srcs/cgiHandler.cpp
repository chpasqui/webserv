#include "../include/Cgi.hpp"
#include "../include/Request.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Client.hpp"

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

#define WRITE_END 1
#define READ_END 0
//https://www.cs.toronto.edu/~penny/teaching/csc309-01f/lectures/40/CGI.pdf

void	Cgi::handleCgi(Client& client, std::string internalPath) // charlotte needs to send this and then the path 
{
	std::cout << GREEN << "ENTERED CGI FUNCTIOM" << RESET << std::endl;
	if (!access(internalPath.c_str(), X_OK)) { return; }
	
	setCgiEnvVariables(client, internalPath);
	buildEnv();
	executeCgi(client, internalPath);
}

void	Cgi::setCgiEnvVariables(const Client& client, std::string internalPath) {
	Request *request = client.getRequest();
	const ServerConfig *config = client.getServerConfig();
	const std::map<std::string, std::string>& header = request->getHeaders();

	_tempEnv.push_back("REQUEST_METHOD=" + request->getMethod());
	_tempEnv.push_back("QUERY_STRING=" + request->getQuery()); // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + request->getVersion());
	_tempEnv.push_back("CONTENT_TYPE=" + getHeaderVariable(header, "content-type"));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	_tempEnv.push_back("PATH_INFO=" + internalPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself 
	
	if (request->getContentLength() != 0) { _tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length")); } // if and onlf if body.. need to check with charlotte
	this ->_scriptName =  internalPath.substr(0, internalPath.find(".py") + 3);
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName); // need to split? not sure how i willr eceive it.... see wiht charlotte today
	_tempEnv.push_back("SERVER_PORT=" + intToString(config->getPort()));
	
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	_tempEnv.push_back("SERVER_NAME=" + std::string(hostname));
}

void	Cgi::buildEnv()
{
	int len;
	for (len = 0; this->_originalEnv[len]; ++len) {}
	int nbEnv = len;

	// const int finalEnvColumns = 2;
	const int finalNbEnv = nbEnv + this->_tempEnv.size();

	this->_finalEnv = new char*[finalNbEnv + 1];
	int index;
	for (index = 0; index < nbEnv; ++index)
	{
		this->_finalEnv[index] = new char[strlen(this->_originalEnv[index]) + 1];
		std::copy(this->_originalEnv[index], this->_originalEnv[index] + strlen(this->_originalEnv[index]), this->_finalEnv[index]);
	}
	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
	{
		this->_finalEnv[index] = new char[(*it).length()];
		strcpy(this->_finalEnv[index], (*it).c_str());
		index++;
	}
	this->_finalEnv[finalNbEnv] = NULL;
}

void	Cgi::executeCgi(Client& client, std::string internalPath)
{
	std::cout << GREEN << "enterex the executeCGI function" << RESET << std::endl;
	Request *request = client.getRequest();
	int fd[2];
	(void)internalPath;
	//const char *execFriendlyPath = internalPath.c_str();
	const char *execFriendlyBuffer = request->getBody().c_str();
	size_t	argSize = request->getBody().size();
	//char	*args[] = { const_cast<char *>(execFriendlyPath), NULL };
	

	if (pipe(fd) == -1) {
		perror("pipe");
		// leave the function and handle error
	}
	pid_t pid = fork();
	if (pid < 0) {
		std::cout << "error" << std::endl;
	}
	if (pid == 0) {                                                                                                                                                                                                                                                      
		close(fd[WRITE_END]);
		dup2(fd[READ_END], STDIN_FILENO);
		close(fd[READ_END]);
		std::cout << GREEN << "inside child all good" << RESET << std::endl;
		exit(1);
		//execve(execFriendlyPath, args, this->_finalEnv);
		//if (execve(internalPath.c_str(), // send someting here, this->_finalEnv) == -1);
	}
	if (pid > 0) {
		close(fd[READ_END]);
		write(fd[WRITE_END], execFriendlyBuffer, argSize);
		close(fd[WRITE_END]);
	}
}

std::string intToString(int number){
	std::stringstream ss;
	ss << number;
	std::string newString;
	ss >> newString;
	return newString;
}

std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& tofind)
{
	std::map<std::string, std::string>::const_iterator it = header.find(tofind);
	if (it != header.end())
		return it->second;
	return "";
}

 


//copy of originalEnv TO DO

// if the method is POST, then i need to pass on the body to the CGI stdin from server -> tell charlotte



//  The server acts as an application gateway.  It receives the request
//    from the client, selects a CGI script to handle the request, converts
//    the client request to a CGI request, executes the script and converts
//    the CGI response into a response for the client.  When processing the
//    client request, it is responsible for implementing any protocol or
//    transport level authentication and security.  The server MAY also
//    function in a 'non-transparent' manner, modifying the request or
//    response in order to provide some additional service, such as media
//    type transformation or protocol reduction.

//CGI defines the abstract parameters, known as metavariables, which describe the client's request.


// If the request has a ./cgi-bin/ inside, then we identify the request as being CGI
// it comes with MEHOD URI (inside the uri is the cgi) and
// first I can check If i receive a directory or a file?
// 1. I need to create a chid process which will handle this --> first before I fork with a child, i need the originalEnv varuables
// od I need to creat a class for CGI?
