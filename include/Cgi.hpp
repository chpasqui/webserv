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
		Cgi(char **originalEnv);
		~Cgi();

		void		handleCgi(Client& client, std::string internalPath);
		void		setCgiEnvVariables(const Client& client, std::string internalPath);
		void		buildEnv();
		void		executeCgi(Client& client, std::string internalPath);
		// std::string extractScriptPath(const std::string& internalPath);
		// std::string getPathInfo(const std::string& internalPath, size_t endScriptName);

	private:
		std::vector<std::string>	_tempEnv;
		char						**_originalEnv;
		char						**_finalEnv;
		int							_tempEnvLength;
		std::string					_scriptName;
		std::string					_pathInfo;
};

std::string intToString(int number);
std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& tofind);

#endif
