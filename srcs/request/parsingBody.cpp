#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"

bool Request::parseBody(const std::string &request, int maxBodySize){
    size_t pos = request.find("\r\n\r\n");
    std::string body = request.substr(pos + 4);
    if(_headers.count("transfer-encoding")){
        if (!unchunkedBody(body))
            return (setError(BAD_REQUEST));
    }
    else if (_headers.count("content-length")){
        if (_body.size() < _contentLength)
            return false; // doit continuer de read, donc si false + statusCode == 200
        if (_contentLength > maxBodySize) //a voir si ce point on le met en dehors pour eviter d envoyer maxBodySize
            return setError(PAYLOAD_TOO_LARGE);
    }
    else
        _contentLength = 0;
    std::cout << _body << std::endl;
    return true;
}

bool Request::unchunkedBody(const std::string &body){

    size_t pos = 0;
    size_t len = body.size();

    while (pos < len){
        size_t ctrf = body.find("\r\n", pos);
        if (ctrf == std::string::npos){
            std::cout << "npos error" << std::endl;
            return false;
        }
        std::string chunkSizeHex = body.substr(pos, ctrf - pos);
        size_t semicolon = chunkSizeHex.find(';');
        if (semicolon != std::string::npos)
        chunkSizeHex =chunkSizeHex.substr(0, semicolon);
        if (chunkSizeHex.empty()){
            std::cout << "size hex empty error" << std::endl;
            return false;
        }
        if (chunkSizeHex.back() == '\r')
            chunkSizeHex.pop_back();
        char *end;
        long chunkSize = std::strtol(chunkSizeHex.c_str(), &end, 16);
        if (*end != '\0' || chunkSize < 0){
            std::cout << *end << std::endl;
            std::cout << "strtol error" << std::endl;
            return false;
        }
        pos = ctrf + 2;
        if (chunkSize == 0){
            break ;
        }
        if (pos + (size_t)chunkSize + 2 > len){
            std::cout << "pos error" << std::endl;
            return false;
        }
        _body += body.substr(pos, chunkSize);
        _contentLength += chunkSize;
        pos += chunkSize;
        if (body.substr(pos, 2) != "\r\n")
            return false;
        pos += 2;
    }
    return true;
}

//from RFC 9112 - 7.1.3 Decoding Chunked
// Decoding Chunked

//   length := 0
//   read chunk-size, chunk-ext (if any), and CRLF
//   while (chunk-size > 0) {
//      read chunk-data and CRLF
//      append chunk-data to content
//      length := length + chunk-size
//      read chunk-size, chunk-ext (if any), and CRLF
//   }
//   read trailer field
//   while (trailer field is not empty) {
//      if (trailer fields are stored/forwarded separately) {
//          append trailer field to existing trailer fields
//      }
//      else if (trailer field is understood and defined as mergeable) {
//          merge trailer field with existing header fields
//      }
//      else {
//          discard trailer field
//      }
//      read trailer field
//   }
//   Content-Length := length
//   Remove "chunked" from Transfer-Encoding

 