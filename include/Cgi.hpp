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
		//Cgi();
		Cgi(const std::string& fileSystemPath);
		~Cgi();
		int			launch(const Request& request, const ServerConfig& config);
	private:
		const std::string&			_fileSystemPath;
		std::vector<std::string>	_tempEnv;
		std::vector<char *>			_finalEnv;

		std::string					_scriptName;
		std::string					_pathInfo;

		void						buildEnv(const Request& request, const ServerConfig& config);
		int							executeScript(const Request& request);
};

std::string getHeaderVariable(const std::map<std::string, std::string>& header, const std::string& key);
std::string parseShebang();
std::string resolveHostname();
std::string intToString(int number);

#endif
