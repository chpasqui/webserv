#include "../../include/Response.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"
#include "../../include/utils.hpp"
#include "../../include/Server.hpp"

#include <ctime>
#include <sstream>
#include <string>
#include <fstream>

static void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

void Response::applySessionTemplate(std::string &html)
{
    double sessionTime = std::difftime(std::time(NULL), _session->getCreatedAt());

    replaceAll(html, "{{USER}}",         _session->data["user"]);
    replaceAll(html, "{{SESSION_ID}}",   _session->getId());
    replaceAll(html, "{{SESSION_TIME}}", intToString(sessionTime));
    replaceAll(html, "{{IS_NEW}}",       _isNewSession ? "true" : "false");
}

void Response::handleLogin(Request& request){
    std::string body = request.getBody();
    std::string prefix = "username=";
    std::string username;

    size_t pos = body.find(prefix);
    if (pos != std::string::npos){
        username = body.substr(pos + prefix.size());
    }
    if (!username.empty())
        _session->data["user"] = username;
    
    _statusCode = FOUND;
    _headers["Location"] = "/";
}

void Response::serverLogoutPage(){
    _sessions->erase(_session->getId());

    _headers["Set-Cookie"] = "session_id=; Max-Age=0; Path=/";
    _statusCode = MOVED_PERMANENTLY;
    _headers["Location"] = "/";
}

