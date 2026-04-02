#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>



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

		bool Request::isValid() const;

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

#endif
