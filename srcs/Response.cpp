#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/statusCode.hpp"

Response::Response() :
    _statusCode(0),
    _reasonPhrase(""),
    _body("") {}

Response::~Response(){}