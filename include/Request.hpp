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
		bool parseHeader();


		bool checkMethod();
		bool checkVersion();
		bool checkSpace(const std::string &line);
		bool parseUrl();
		void stockIfQuery(std::string url_decode);

	private:
		std::string _method;
		std::string _version;
		std::string _url;
		std::string _path;
		std::string _body;
		std::string _queryString;
		std::string	_requestMessage;
		int			_statusCode;
		int 		_contentLength;
	
		std::map<std::string, int> _headers;
};

#endif
