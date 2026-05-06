#include "../../include/Request.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"


static bool parseChunkSize(const std::string &body, size_t &pos, long &chunkSize);

/**
 * @brief Parses body based on transfer-encoding or content-length header.
 * @param body The raw request buffer.
 * @param maxBodySize Maximum allowed body size.
 */
bool Request::parseBody(const std::string &body, int maxBodySize){
    
    if (_headers.count("transfer-encoding")){
        if (!unchunkedBody(body))
            return(setError(BAD_REQUEST, "parseBody: unchunked failed"));
        return true ;
    }
    else if (_headers.count("content-length")){
        if (_contentLength > maxBodySize) 
            return (setError(PAYLOAD_TOO_LARGE, "parseBody: contentLength too long"));
        _body = body;
        return true ;
    }
    _contentLength = 0;
    return true ;
}

/**
 * @brief Decodes a chunked transfer-encoding body and stores it in _body.
 * @param body The raw chunked body string.
 * @return true on success, false on error with statusCode updated.
 */
bool Request::unchunkedBody(const std::string &body){

    size_t pos = 0;
    long chunkSize = 0;
    _contentLength = 0;

    while (pos < body.size()){
        if (!parseChunkSize(body, pos, chunkSize))
            return false;
        if (chunkSize == 0){
            break ;
        }
        if (!extractChunkData(body, pos, chunkSize))
            return false;
    }
    return true;
} 

/**
 * @brief Extracts and parses chunk size from hex string, strips extensions.
 * @param body The raw body buffer.
 * @param pos Current position in buffer.
 * @param chunkSize Output parsed chunk size.
 * @return true on success, false on error.
 */
static bool parseChunkSize(const std::string &body, size_t &pos, long &chunkSize){
    size_t ctrf = body.find("\r\n", pos);
    if (ctrf == std::string::npos)
        return false;
    std::string chunkSizeHex = body.substr(pos, ctrf - pos);
    size_t semicolon = chunkSizeHex.find(';');
    if (semicolon != std::string::npos)
    chunkSizeHex =chunkSizeHex.substr(0, semicolon);
    if (chunkSizeHex.empty())
        return false;
    std::string::iterator it = chunkSizeHex.end() - 1;
    if (*it == '\r')
        chunkSizeHex.erase(it);
    char *end;
    chunkSize = strtol(chunkSizeHex.c_str(), &end, 16);
    if (*end != '\0' || chunkSize < 0)
        return false;
    pos = ctrf + 2;
    return true ;
}

/**
 * @brief Extracts chunk data and validates trailing CRLF.
 * @param body The raw body buffer.
 * @param pos Current position in buffer.
 * @param chunkSize Size of chunk to extract.
 * @return true on success, false on error.
 */
bool Request::extractChunkData(const std::string &body, size_t &pos, long chunkSize){
    if (pos + (size_t)chunkSize + 2 > body.size())
            return false;
        _body += body.substr(pos, chunkSize);
        _contentLength += chunkSize;
        pos += chunkSize;
        if (body.substr(pos, 2) != "\r\n")
            return false;
        pos += 2;
        return true ;
    }

