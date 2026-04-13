#ifndef CGI_HPP
# define CGI_HPP

#include "Response.hpp"
#include "Request.hpp"

class Cgi
{
private:
	Response *response; // would be allocated on the stack and doesnt work
	std::map<std::string, std::string> _env; // for now this is all I know
public:
	Cgi();
	~Cgi();
	void	handleCgi(Request *request, std::string cgiPath)
};

#endif
