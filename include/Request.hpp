#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <string>

class Request
{
	public:
		Request();
		~Request();

		//Getter
		int					getStatusCode(void) const;
		int					getContentLength(void) const;
		const std::string& 	getVersion(void) const;
		const std::string& 	getBody(void) const;
		const std::string& 	getPath(void) const;
		const std::string& 	getMethod(void) const;
		const std::string& 	getHeader(std::string key) const;
		std::string getUrl(void) const;			// eventuellement faire une classe Request a part avec const
		std::string getQuery(void) const;
		const std::map<std::string, std::string>& getHeaders() const;

		//Parsing
		bool parseRequestLine(const std::string &message);
		bool parseHeader(const std::string &request);
		bool validateHeader();
		bool parseBody(const std::string &request);

		void setMaxBodySize(int maxBodySize);
		bool setError(int code, const std::string &context);

	private:
		std::string _method;
		std::string _version;
		std::string _url;
		std::string _path;
		std::string _body;
		std::string _query;
		int			_contentLength;
		int			_maxBodySize;
		int			_statusCode;
	
		std::map<std::string, std::string> _headers;

		// Request-line
		bool validSynthax(const std::string &line);
		bool validMethod();
		bool validVersion();
		bool validUrl();
		void stockIfQuery(std::string url_decode);

		// Header
		bool 		validateCRLF(const std::string &line);
		bool 		validateObsFold(const std::string &line);
		bool 		validateFieldName(const std::string &line, size_t colonPos);
		std::string extractFieldName(const std::string &line, size_t colonPos);
		std::string extractValue(const std::string &line, size_t colonPos);
		bool 		validateFieldValue(const std::string &fieldname, const std::string &value);

		// Body
		bool unchunkedBody(const std::string &body);
		bool extractChunkData(const std::string &body, size_t &pos, long chunkSize);

};

std::string decodeUrl(const std::string &url);
bool        checkUnsafeChar(const std::string &url_decode);
bool        isTokenChar(unsigned char c);

#endif
