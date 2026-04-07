#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"

bool Request::parseBody(const std::string &request){
    size_t start = request.find("\r\n\r\n" + 4);
    std::string body = request.substr(start);
    if(_headers.count("transfer-encoding"))
        _body = unchunkedBody(body);
    else if (_headers.count("content-length"))
        
    
    
}


std::string Request::unchunkedBody(const std::string &body){

}
     length := 0
     read chunk-size, chunk-ext (if any), and CRLF
     while (chunk-size > 0) {
        read chunk-data and CRLF
        append chunk-data to decoded-body
        length := length + chunk-size
        read chunk-size, chunk-ext (if any), and CRLF
     }
     read trailer field
     while (trailer field is not empty) {
        if (trailer field is allowed to be sent in a trailer) {
            append trailer field to existing header fields
        }
        read trailer-field
     }
     Content-Length := length
     Remove "chunked" from Transfer-Encoding
     Remove Trailer from existing header fields