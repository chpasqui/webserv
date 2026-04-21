#include "../include/Cgi.hpp"
#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Client.hpp"

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

#define WRITE_END 1
#define READ_END 0

//https://www.cs.toronto.edu/~penny/teaching/csc309-01f/lectures/40/CGI.pdf

int	Cgi::executeCgi(const Request& request, const ServerConfig& config, const std::string& internalPath) // charlotte needs to send this and then the path 
{
	std::cout << GREEN << "ENTERED CGI FUNCTIOM" << RESET << std::endl;

	// check if i have access to the path 
	if (access(internalPath.c_str(), X_OK) == -1) {
		std::cerr << "access issue" << std::endl;
		return -1;
	}
	
	setCgiEnvVariables(request, config, internalPath);
	return (executeScript(request, internalPath));
	//return -1;
}

/**
 * @brief 
 *  Environment variables are used to pass data about the request from
 * the server to the script. They are accessed by the script in a system 
 * defined manner. In all cases, a missing environment variable is
 * equivalent to a zero-length (NULL) value, and vice versa.
 * 
 * PATH_INFO identifies the resource to be returnwed by the CGI script. The leading / is not part of the pathinfo
 * if it is just "/", then it must be replaced by "".
 * 
 * 
 * @param client 
 * @param internalPath 
 */
void	Cgi::setCgiEnvVariables(const Request& request, const ServerConfig& config, const std::string& internalPath) {
	
	const std::map<std::string, std::string>& header = request.getHeaders();

	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("REQUEST_METHOD=" + request.getMethod());
	_tempEnv.push_back("QUERY_STRING=" + request.getQuery()); // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + request.getVersion());
	_tempEnv.push_back("CONTENT_TYPE=" + getHeaderVariable(header, "content-type"));
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	
	if (request.getContentLength() != 0) { _tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length")); } // if and onlf if body.. need to check with charlotte
	this ->_scriptName =  internalPath.substr(0, internalPath.find(".py") + 3);
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName); // need to split? not sure how i willr eceive it.... see wiht charlotte today
	_tempEnv.push_back("PATH_INFO=" + internalPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself 
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	_tempEnv.push_back("SERVER_NAME=" + std::string(hostname));
	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
		std::cout << GREEN << *it << RESET << std::endl;
}

/**
 * @brief 
 * 
 */
char**	Cgi::setEnv()
{
	char **env = new char*[_tempEnv.size() + 1];
	int index = 0;
	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
	{
		env[index] = new char[(*it).length() + 1];
		strcpy(env[index], (*it).c_str());
		index++;
	}
	env[_tempEnv.size() + 1] = NULL;
	return env;
}



// Create the new fd
// add it to the epoll (the pipe out)
// 
/**
 * @brief 
 * 
 * @param client 
 * @param internalPath 
 * 
 * commands for the CGI would be 
 */
int	Cgi::executeScript(const Request& request, const std::string &internalPath)
{
	(void)request;
	std::cout << GREEN << "entered the executeCGI function" << RESET << std::endl;

	int fd[2];
	
	if (pipe(fd) == -1) {
		perror("pipe");
		// leave the function and handle error
	}
	fcntl(fd[0], F_SETFD | O_NONBLOCK); // set the read end of the pipe to non blocking
	pid_t pid = fork();
	if (pid < 0) {
		std::cout << "error" << std::endl;
	}
	if (pid == 0) {

		std::cout << "entered the child" << std::endl;
		std::string interpreterStr = extractExecutable();
		this->_interpreter = interpreterStr.c_str();
		const char *scriptpath = internalPath.c_str();
		std::cout << "scriptpath :" << scriptpath << std::endl;
		const char *args[] = { this->_interpreter, scriptpath, NULL };


		int tempFileFd = open("/tools/tmpfile.txt", O_RDONLY);
		std::cout << GREEN << "inside child all good" << RESET << std::endl;
		dup2(tempFileFd, STDIN_FILENO);
		dup2(fd[WRITE_END], STDOUT_FILENO);
		close(fd[WRITE_END]);
		close(fd[READ_END]);
		execve(this->_interpreter, const_cast<char *const *>(args), setEnv());
		perror("Error: execve was not able to do its thing");
		exit(1);
	}
	else {
		close(fd[WRITE_END]);
		return fd[READ_END];
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

std::string extractExecutable() {
	std::ifstream filestream("./www/cgi-bin/hello.py");
	std::string line;
	getline(filestream, line);
	std::cout << "line contains " << line << std::endl;
	if (line.substr(0,2) != "#!")
	{
		std::cout << "CASE 1" << std::endl;
		return "";
	}

	if (line.find("env") != std::string::npos) {
		std::cout << "CASE 2" << std::endl;
		size_t spacepos = line.find(' ');
		std::string interpreterName = line.substr(spacepos + 1);
		return "/usr/bin/" + interpreterName; // /usr/bin/python3
	}
	std::cout << "CASE 3" << std::endl;
	
	return line;
}
