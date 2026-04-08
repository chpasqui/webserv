#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"

void Request::parseBody(const std::string &body, int maxBodySize){
    
    if (_headers.count("transfer-encoding")){
        if (!unchunkedBody(body)){
            _statusCode = BAD_REQUEST;
            return ;
        }
    }
    else if (_headers.count("content-length")){
        if (_contentLength > maxBodySize){ //a voir si ce point on le met en dehors pour eviter d envoyer maxBodySize
            _statusCode = PAYLOAD_TOO_LARGE;
            return ;
        }
        _body = body;
    }
    else
        _body = "";
    std::cout << _body << std::endl; //si pas de body du tout, initialisation de _body vide ou traiter autrement pour l attribut
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
        long chunkSize = strtol(chunkSizeHex.c_str(), &end, 16);
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

 