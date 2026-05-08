#include "../../include/Response.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"
#include "../../include/Cgi.hpp"

#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <algorithm>

/**
 * @brief Builds the HTTP response from a parsed request.
 * Resolves location, checks method, dispatches to handler or CGI.
 */
void Response::build(Request &request, const ServerConfig *config)
{
	_config = config;
	_loc = findLocation(request.getPath());
	if (request.getStatusCode() != 0)
	{
		setError(request.getStatusCode());
		return;
	}
	if (request.getContentLength() > _loc->getMaxBodySize())
		return (setError(PAYLOAD_TOO_LARGE));
	_internalPath = buildFileSystemPath(request.getPath());
	if (_loc->getRedirCode() != 0)
	{
		_statusCode = _loc->getRedirCode();
		_headers["Location"] = _loc->getRedirPath();
		return;
	}
	if (!methodAllowed(request.getMethod()))
	{
		setError(METHOD_NOT_ALLOWED);
		return;
	}
	if (isCGI())
	{
		this->_isCgi = true;
		return;
	}
	handleMethod(request);
}

/**
 * @brief Returns the longest-prefix matching location for the given URL.
 * @return Pointer to matching Location, or NULL if none found.
 */
Location *Response::findLocation(const std::string &url)
{
	Location *best = NULL;
	size_t bestLen = 0;

	std::vector<Location *> locs = _config->getLocations();
	std::vector<Location *>::iterator it = locs.begin();
	for (; it != locs.end(); it++)
	{
		const std::string &prefix = (*it)->getPath();
		if (url.substr(0, prefix.size()) == prefix && prefix.size() > bestLen)
		{
			best = *it;
			bestLen = prefix.size();
		}
	}
	return best;
}

/**
 * @brief Builds the filesystem path from location root and URL remainder.
 */
std::string Response::buildFileSystemPath(const std::string &url)
{
	const std::string& prefix = _loc->getPath();
	_remain = url.substr(prefix.size());
	if (!_remain.empty() && _remain[0] == '/')
		_remain = _remain.substr(1);
	LOG(std::cout << "prefix='" << prefix << "' remain='" << _remain << "' path='" << _loc->getRoot() + _remain << "'" << std::endl);
	return _loc->getRoot() + _remain;
}

/**
 * @brief Checks if the method is allowed by the matched location.
 */
bool Response::methodAllowed(const std::string &method)
{
	std::string methodAllowed = _loc->getAllowedMethods();
	if (methodAllowed.find(method) != std::string::npos)
		return true;
	return false;
}

/**
 * @brief Checks if the request targets a CGI resource by extension.
 */
bool Response::isCGI()
{
	std::map<std::string, std::string> temp = this->_loc->getCgiExtensions();
	std::map<std::string, std::string>::iterator it = temp.begin();
	for (; it != temp.end(); it++)
	{
		size_t pos = _internalPath.find(it->first);
		if (pos != std::string::npos)
		{
			size_t end = pos + it->first.size();
			if (end == _internalPath.size() || _internalPath[end] == '/')
			{
				_cgiPass = it->second;
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Dispatches to the appropriate method handler (GET/POST/DELETE).
 */
void Response::handleMethod(Request &request)
{
	if (request.getMethod() == "GET")
		handleGetMethod(request);
	else if (request.getMethod() == "POST")
	{
		if (request.getPath() == "/login")
			handleLogin(request);
		else
			handlePostMethod(request);
	}
	else
		handleDeleteMethod(request);
}

/**
 * @brief Sets _statusCode from errno and calls buildError().
 */
void Response::setSyscallError()
{
	if (errno == ENOENT || errno == ENOTDIR)
		setError(NOT_FOUND);
	else if (errno == EACCES)
		setError(FORBIDDEN);
	else
		setError(INTERNAL_SERVER_ERROR);
}

void normalizeHeaders(std::string& name){
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		name[0] = toupper(name[0]);
		for (size_t i = 0; i < name.length(); i++){
			if (name[i] == '-')
				name[i + 1] = toupper(name[i + 1]);
		}
}

void Response::parseCgiHeaders(std::string &headerSection)
{
	std::string line;
	std::istringstream split(headerSection);

	while (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue;

		std::string name = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		normalizeHeaders(name);

		size_t start = value.find_first_not_of(' ');
		if (start != std::string::npos)
			value = value.substr(start);

		_headers[name] = value;
	}
}

bool Response::splitOutput(const std::string &output, std::string &headerSection)
{
	size_t pos = output.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		headerSection = output.substr(0, pos);
		_body = output.substr(pos + 4);
		return true;
	}
	pos = output.find("\n\n");
	if (pos != std::string::npos)
	{
		headerSection = output.substr(0, pos);
		_body = output.substr(pos + 2);
		return true;
	}
	return false;
}

void Response::buildCgiResponse(const std::string &output)
{
	std::string headerSection;

	if (!splitOutput(output, headerSection))
	{
		setError(INTERNAL_SERVER_ERROR);
		return;
	}
	parseCgiHeaders(headerSection);
	if (!_body.empty() && !_headers.count("Content-Type"))
		setError(INTERNAL_SERVER_ERROR);
	if (_headers.count("status"))
	{
		_statusCode = std::atoi(_headers["Status"].c_str());
		_headers.erase("status");
	}
	else
		_statusCode = OK;
}


/**
 * @brief Populates _body with a custom or generated error page.
 */
void Response::buildError()
{

	std::string errorPageFile;
	if (!_loc->getErrorPage(_statusCode).empty())
		errorPageFile = _loc->getRoot() + "/" + _loc->getErrorPage(_statusCode);
	if (!errorPageFile.empty())
	{
		struct stat errorPath;
		if (stat(errorPageFile.c_str(), &errorPath) == 0 && S_ISREG(errorPath.st_mode))
		{
			std::ifstream file(errorPageFile.c_str());
			if (file.is_open())
			{
				std::ostringstream ss;
				ss << file.rdbuf();
				_body = ss.str();
				_headers["Content-Type"] = "text/html";
				return;
			}
		}
	}
	generateErrorPage();
}

/**
 * @brief Generates a minimal HTML error page from _statusCode.
 */
void Response::generateErrorPage()
{
	std::ostringstream body;

	body << "<html>\n<head><title>" << _statusCode << " " << getReasonPhrase() << "</title></head>\n"
		 << "<body>\n"
		 << "<h1>" << _statusCode << " " << getReasonPhrase() << "</h1>\n"
		 << "<hr>\n"
		 << "<p>webserv</p>\n"
		 << "</body>\n</html>\n";

	_body = body.str();
	_headers["Content-Type"] = "text/html";
}
