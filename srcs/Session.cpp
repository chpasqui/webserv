#include "Session.hpp"

#include <sstream>
#include <ctime>

 Session::Session(){}

//Step 1: ce sera dans serveur, mais si aucun header cookie, generer un sessionId
//Step 2: si y a un header Cookie, il faut que j'extracte le sessionId 
//Step 3: dans buildReponse aller voir si y a un cookie
//Step 4: rajouter le headerCookie dans la responseFormat

std::string Session::generateId(){
    std::ostringstream ss;
    std::time_t currentTime = std::time(NULL);
    ss << std::hex << currentTime << std::rand();
    return ss.str();
}

//mais en fait est ce que jaurai bien la key session_id, est ce que cest parce que cest ca que je cree moi dans le server?
std::string Session::extractSessionId(std::string cookieHeader){
	std::string key = "session_id=";
	size_t pos = cookieHeader.find(key);
	if (pos == std::string::npos)
		return "";
    pos += key.size();
    size_t end = cookieHeader.find(";", pos);
    if (end == std::string::npos)
        return cookieHeader.substr(pos);
    return cookieHeader.substr(pos, end - pos);
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

Session::~Session(){}


