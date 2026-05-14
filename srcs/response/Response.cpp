#include "../../include/Response.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"

Response::Response() :
    _config(NULL),
    _loc(NULL),
    _statusCode(0),
    _body(""), 
    _session(NULL),
    _isNewSession(false),
	_isCgi(false), 
    _cgiPass("") {}

Response::Response(Session& session, bool isNew) : 
    _config(NULL),
    _loc(NULL),
    _statusCode(0),
    _body(""),
    _session(&session),
    _isNewSession(isNew),
	_isCgi(false),
    _cgiPass("") {}

Response::~Response(){}

/**
 * @brief Returns the reason phrase for the current _statusCode.
 */
std::string Response::getReasonPhrase() const{
    switch(_statusCode) {
        case OK : return "Success";
        case CREATED : return "Created";
        case NO_CONTENT : return "No content";
        case MOVED_PERMANENTLY : return "Moved Permanently";
        case BAD_REQUEST : return "Bad Request";
        case UNAUTHORIZED : return "Unauthorized";
        case FORBIDDEN : return "Forbidden";
        case NOT_FOUND : return "Not found";
        case METHOD_NOT_ALLOWED : return "Method not allowed";
        case REQUEST_TIMEOUT : return "Request Timeout";
        case CONFLICT : return "Conflict";
        case LENGTH_REQUIRED : return "Length required";
        case PAYLOAD_TOO_LARGE : return "Payload too large";
        case URL_TOOLONG : return "Url too long";
        case INTERNAL_SERVER_ERROR : return "Internal server loop";
        case NOT_IMPLEMENTED : return "Not implemented";
        case WRONG_HTTP : return "Wrong HTTP";
        default : return "Error";

    }
}

/**
 * @brief Infers Content type from _internalPath extension.
 * @return Content type string, defaults to application/octet-stream.
 */
// est ce qu'il y en a d'autres a mettre ?
std::string Response::extractContentType(){
    static std::map<std::string, std::string> mime;
    if (mime.empty()) {
        mime[".html"] = "text/html";
        mime[".css"]  = "text/css";
        mime[".js"]   = "application/javascript";
        mime[".png"]  = "image/png";
        mime[".jpg"]  = "image/jpeg";
        mime[".txt"]  = "text/plain";
    }
    size_t dot = _internalPath.rfind('.');
    if (dot == std::string::npos)
        return "application/octet-stream";
    std::map<std::string, std::string>::iterator it = mime.find(_internalPath.substr(dot));
    if (it == mime.end())
        return "application/octet-stream";
    return it->second;   
}

/**
 * @brief Sets Content-Length and Server headers from current _body.
 */
// To decide whether we want to put the date or not
void Response::setDefaultHeaders(){
    if (_headers.count("Content-Length") == 0 && !_body.empty()){
        std::ostringstream ss;
        ss << _body.size();
        _headers["Content-Length"] = ss.str();
    }
    if (_headers.count("Server") == 0)
        _headers["Server"] = "webserv";
}

bool	Response::getCgiStatus(void) const {
    return this->_isCgi;
}

std::string    Response::getInternalPath(void) const {
    return this->_internalPath;
}

std::string	Response::getCgiPass(void) const{
    return this->_cgiPass;
}

void Response::setError(int code, const std::string &context) {
    _statusCode = code;
    LOG_ERR(code, context);
    (void)context;
    buildError();
}