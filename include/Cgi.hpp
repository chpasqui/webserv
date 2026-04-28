#ifndef CGI_HPP
# define CGI_HPP

#include "Client.hpp"
#include "ServerConfig.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <string>
#include <vector>

#ifndef COLORS
#define COLORS
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#endif

class Cgi
{
	public:
		Cgi();
		~Cgi();
		int			executeCgi(const Request& request, const ServerConfig& config, const std::string& internalPath);
	private:
		std::vector<std::string>	_tempEnv;
		int							_tempEnvLength;
		std::string					_scriptName;
		std::string					_pathInfo;
		const char *				_interpreter;
		std::vector<char *>			setCgiEnvVariables(const Request& request, const ServerConfig& config, const std::string& internalPath);
		int							executeScript(const Request& request, const std::string &internalPath);
};

std::string intToString(int number);
std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& tofind);
std::string extractExecutable();

#endif
