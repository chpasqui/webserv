#include "../include/Request.hpp"
#include "../include/statusCode.hpp"
#include "../include/Debug.hpp"

#include <vector>


Request::Request() :
    _method(""),
	_version(""),
	_url(""),
     _path(""),
     _body(""),
     _query(""),
     _contentLength(-1),
     _statusCode(0) {}

Request::~Request(){}

int	Request::getStatusCode(void) const{
	return (_statusCode);
}

int	Request::getContentLength(void) const{
	return (_contentLength);
}

const std::string& Request::getPath(void) const{
    return (_path);
}

const std::string& Request::getMethod(void) const{
    return (_method);
}

const std::string& Request::getBody(void) const{
    return (_body);
}

const std::string& Request::getHeaders(std::string key) const{
    std::map<std::string, std::string>::const_iterator it = _headers.begin();
	for(; it != _headers.end(); it++)
        if (it->first == key)
            return it->second;
    return "";
}

/**
 * @brief Sets _statusCode and returns false, used to propagate errors.
 * @param code HTTP status code to set.
 * @return Always false.
 */
bool Request::setError(int code, const std::string &context) {
    _statusCode = code;
    LOG_ERR(code, context);
    (void)context;
    return false;
}