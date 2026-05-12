#ifndef CGI_HPP
#define CGI_HPP

#include "../include/Client.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Response.hpp"
#include "../include/Request.hpp"
#include <string>
#include <vector>

#ifndef COLORS
#define COLORS
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"
#endif

extern void safeClose(int fd);

class Cgi
{
	public:
		// Cgi();
		Cgi();
		Cgi(Response *response, Request *request);
		~Cgi();
		void launch(const ServerConfig &config);
		int	getStdinWriteEnd() const;
		int getStdoutReadEnd() const;
		int getPid(void) const;

	private:
		std::string _fileSystemPath;
		std::vector<std::string> _tempEnv;
		std::vector<char *> _finalEnv;

		std::string _scriptName;
		std::string _pathInfo;
		std::string _fileName;

		int _stdinWriteEnd;
		int _stdoutReadEnd;
		int _pid;

		void buildEnv(const ServerConfig &config);
		void executeScript();
		void openPipes(int *stdinPipe, int * stdoutPipe);

		Response *_response;
		Request *_request;
};

std::string getHeaderVariable(const std::map<std::string, std::string> &header, const std::string &key);
std::string parseShebang(const std::string &internalPath);
std::string resolveHostname();
std::string intToString(int number);

#endif
