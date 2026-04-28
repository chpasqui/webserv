#include "../include/Cgi.hpp"
#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Client.hpp"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <cstdlib>

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
std::vector<char *>	Cgi::setCgiEnvVariables(const Request& request, const ServerConfig& config, const std::string& internalPath) {
	
	const std::map<std::string, std::string>& header = request.getHeaders();

	_tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_tempEnv.push_back("REQUEST_METHOD=" + request.getMethod());
	_tempEnv.push_back("QUERY_STRING=" + request.getQuery()); // query string must be defined as empty string if not found (treated by getHeaderVariable)
	_tempEnv.push_back("SERVER_PROTOCOL=" + request.getVersion());
	_tempEnv.push_back("HTTP_HOST=" + getHeaderVariable(header, "host"));
	
	if (request.getContentLength() != 0) { _tempEnv.push_back("CONTENT_LENGTH=" + getHeaderVariable(header, "content-length")); } // if and onlf if body.. need to check with charlotte
	this ->_scriptName =  internalPath.substr(0, internalPath.find(".py") + 3);
	if (request.getMethod() == "POST") { _tempEnv.push_back("CONTENT_TYPE=" + getHeaderVariable(header, "content-type")); }
	_tempEnv.push_back("SCRIPT_NAME=" + this->_scriptName); // need to split? not sure how i willr eceive it.... see wiht charlotte today
	_tempEnv.push_back("PATH_INFO=" + internalPath.substr(_scriptName.length())); // derived from the portion of the uri path following the part that identifies the script itself 
	_tempEnv.push_back("SERVER_PORT=" + intToString(config.getPort()));
	
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	_tempEnv.push_back("SERVER_NAME=" + std::string(hostname));
		std::vector<char *> envp;
	for (std::vector<std::string>::iterator it = _tempEnv.begin(); it != _tempEnv.end(); ++it)
		envp.push_back(const_cast<char *>((*it).c_str()));

	envp.push_back(NULL);
	return envp;
}

std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& tofind)
{
	std::map<std::string, std::string>::const_iterator it = header.find(tofind);
	if (it != header.end())
		return it->second;
	return "";
}

std::string extractExecutable(const std::string& internalPath) {
	std::ifstream filestream(internalPath.c_str());
	std::string line;
	getline(filestream, line);
	if (line.substr(0,2) != "#!")
	{
		return "";
	}

	if (line.find("env") != std::string::npos) {
		size_t spacepos = line.find(' ');
		std::string interpreterName = line.substr(spacepos + 1);
		return "/usr/bin/" + interpreterName; // /usr/bin/python3
	}
	
	return line;
}

