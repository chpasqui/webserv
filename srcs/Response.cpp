#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"

Response::Response() :
    _config(NULL),
    _location(NULL),
    _statusCode(0),
    _reasonPhrase(""),
    _body("") {}

Response::~Response(){}