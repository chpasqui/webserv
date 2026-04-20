#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"

Response::Response() :
    _config(NULL),
    _loc(NULL),
    _statusCode(0),
    _body("") {}

Response::~Response(){}

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

// To decide whether we want to put the date or not
void Response::setDefaultHeaders(){
    std::ostringstream ss;
    ss << _body.size();
    _headers["Content-Length"] = ss.str();
    _headers["Server"] = "webserv";
}