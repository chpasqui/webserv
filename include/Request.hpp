#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>

class Request
{
	public:
		Request();
		~Request();

		int	getStatusCode(void) const;
		int	getContentLength(void) const;

		bool parseRequestLine(const std::string &message);
		bool validSynthax(const std::string &line);
		bool validMethod();
		bool validVersion();
		bool validUrl();
		std::string decodeUrl(std::string url);
		bool checkUnsafeChar(std::string url_decode);
		void stockIfQuery(std::string url_decode);

		bool parseHeader(const std::string &request);
		bool isTokenChar(unsigned char c);
		bool validateCRLF(const std::string &line);
		bool validateObsFold(const std::string &line);
		bool validateFieldName(const std::string &line, size_t colonPos);
		std::string extractFieldName(const std::string &line, size_t colonPos);
		std::string extractValue(const std::string &line, size_t colonPos);
		bool validateFieldValue(const std::string &fieldname, const std::string &value);
		bool setError(int code, const std::string &context);

		bool validateHeader();

		bool parseBody(const std::string &request, int maxBodySize);
		bool unchunkedBody(const std::string &body);

		int	getStatusCode(void) const;
		int	getContentLength(void) const;
		std::string getMethod(void) const;
		std::string getUrl(void) const;			// eventuellement faire une classe Request a part avec const
		std::string getPath(void) const;		// dest et getters pour trouver facilement
		std::string getQuery(void) const;
		std::string getVersion(void) const;
		const std::string& getBody(void) const;
		const std::map<std::string, std::string>& getHeaders() const;

		bool extractChunkData(const std::string &body, size_t &pos, long chunkSize);

		private:
		std::string _method;
		std::string _version;
		std::string _url;
		std::string _path;
		std::string _body;
		std::string _query;
		int			_contentLength;
		int			_statusCode;
	
		std::map<std::string, std::string> _headers;

		// Request-line
		bool validSynthax(const std::string &line);
		bool validMethod();
		bool validVersion();
		bool validUrl();
		void stockIfQuery(std::string url_decode);

		// Header
		bool validateCRLF(const std::string &line);
		bool validateObsFold(const std::string &line);
		bool validateFieldName(const std::string &line, size_t colonPos);
		std::string extractFieldName(const std::string &line, size_t colonPos);
		std::string extractValue(const std::string &line, size_t colonPos);
		bool validateFieldValue(const std::string &fieldname, const std::string &value);

		// Body
		bool unchunkedBody(const std::string &body);
		bool extractChunkData(const std::string &body, size_t &pos, long chunkSize);

		// Utils
		bool setError(int code, const std::string &context);

};

std::string decodeUrl(const std::string &url);
bool        checkUnsafeChar(const std::string &url_decode);
bool        isTokenChar(unsigned char c);
void 		debugListHeader(std::map<std::string, std::string> &_headers);

#endif
