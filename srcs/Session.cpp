#include "../include/Session.hpp"

#include <sstream>
#include <ctime>
#include <cstdlib>

 Session::Session() : _createdAt(std::time(NULL)) {}

std::string Session::generateId(){
    std::ostringstream ss;
    std::time_t currentTime = std::time(NULL);
    ss << std::hex << currentTime << std::rand();
    return ss.str();
}

std::string Session::extractSessionId(std::string cookieHeader){
	std::string key = "session_id=";
	size_t pos = cookieHeader.find(key);
	while (pos != std::string::npos){
		if (pos == 0 || (pos >= 2 && cookieHeader.substr(pos - 2, 2) == "; ")){
			pos += key.size();
			size_t end = cookieHeader.find(";", pos);
			if (end == std::string::npos)
				return cookieHeader.substr(pos);
			return cookieHeader.substr(pos, end - pos);
		}
		pos = cookieHeader.find(key, pos + 1);
	}
	return "";
}

std::string Session::toSetCookieString() const
{
    return "session_id=" + _id + "; Path=/";
}

void Session::setId(const std::string& id){
    _id = id;
}

std::string Session::getId() const{
    return (_id);
}

time_t Session::getCreatedAt() const{
	return (_createdAt);
}

Session::~Session(){}


