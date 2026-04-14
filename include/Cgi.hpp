#ifndef CGI_HPP
# define CGI_HPP

#include "Response.hpp"
#include "Request.hpp"

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
		Cgi(char **env);
		~Cgi();

		void	handleCgi(Request *request, std::string cgiPath);
		void	buildCgiEnv(Request *request);


	private:
		//Response *response; // would be allocated on the stack and doesnt work
		std::map<std::string, std::string> _env; // for now this is all I know
		char	**env;
};

#endif
