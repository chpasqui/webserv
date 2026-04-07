#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>

class Request
{
	public:
		Request();
		Request(std::string requestMessage);
		~Request();

		bool parseRequestLine(const std::string &message);
		bool validSynthax(const std::string &line);
		bool validMethod();
		bool validVersion();
		bool validUrl();
		void stockIfQuery(std::string url_decode);

		bool parseHeader(const std::string &request);
		bool validateCRLF(const std::string &line);
		bool validateObsFold(const std::string &line);
		bool validateFieldName(const std::string &line, size_t colonPos);
		std::string extractFieldName(const std::string &line, size_t colonPos);
		std::string extractValue(const std::string &line, size_t colonPos);
		bool validateFieldValue(const std::string &fieldname, const std::string &value);
		bool setError(int code);

		bool validateHeader();

		bool parseBody(const std::string &request, int maxBodySize);
		bool unchunkedBody(const std::string &body);

	private:
		std::string _method;
		std::string _version;
		std::string _url;
		std::string _path;
		std::string _body;
		std::string _query;
		std::string _requestMessage;
		int			_contentLength;
		int			_statusCode;
	
		std::map<std::string, std::string> _headers;
};

void debugListHeader(std::map<std::string, std::string> &_headers);

#endif
