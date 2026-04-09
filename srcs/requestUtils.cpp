#include "../include/Request.hpp"
#include "../include/statusCode.hpp"
#include "../include/Debug.hpp"

/**
 * @brief Decodes %XX sequences to ASCII.
 * @param url The raw URL string.
 * @return Decoded string, or "-1" if control character detected.
 */
std::string decodeUrl(const std::string& url){

	std::string decoded;
	for (size_t i = 0; i < url.size(); i++){
		if (url[i] == '%' && i + 2 < url.size()){
			std::string hex = url.substr(i + 1, 2);
			long ascii = strtol(hex.c_str(), NULL, 16);
			//std::cout << ascii << std::endl;
			if ((ascii >= 0 && ascii < 32) || ascii == 127)
				return ("-1");
			decoded += static_cast<char>(ascii);
			i += 2;
		}
		else 
			decoded += url[i];
	}
	return decoded;
}

/**
 * @brief Checks for unsafe characters and path traversal attempts.
 * @param url_decode The decoded URL string.
 * @return true if safe, false otherwise.
 */
bool checkUnsafeChar(const std::string& url_decode){

	if (url_decode == "-1" || url_decode.find('%') != std::string::npos ||
		url_decode.find('#') != std::string::npos || url_decode.find('<') != std::string::npos || 
		url_decode.find('<') != std::string::npos || url_decode.find("/../") != std::string::npos)
			return false;
    return true;
}

/**
 * @brief Checks if character is a valid token character per RFC 7230.
 * @param c Character to check.
 * @return true if valid, false otherwise.
 */
bool isTokenChar(unsigned char c){
	if(isalnum(c))
		return true;
	std::string special = "!#$%&'*+-.^_`|~";
	return (special.find(c) != std::string::npos);
}
