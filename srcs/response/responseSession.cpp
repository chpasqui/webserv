#include "../../include/Response.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"
#include "../../include/utils.hpp"
#include "../../include/Server.hpp"

#include <ctime>
#include <sstream>
#include <string>

static std::string readFile(const std::string &path)
{
    std::ifstream f(path.c_str());
    if (!f.is_open())
        return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

void Response::serverSessionPage()
{
    std::string tmpl = readFile("www/index.html");

    if (tmpl.empty())
    {
        _body = "<html><body><p>Session page unavailable</p></body></html>";
        _statusCode = INTERNAL_SERVER_ERROR;
        _headers["Content-Type"] = "text/html";
        return;
    }

    double sessionTime = std::difftime(std::time(NULL), _session->getCreatedAt());
    std::string user        = _session->data["user"];
    std::string sessionId   = _session->getId();

    replaceAll(tmpl, "{{USER}}",         user);
    replaceAll(tmpl, "{{SESSION_ID}}",   sessionId);
    replaceAll(tmpl, "{{SESSION_TIME}}", intToString(sessionTime));
    replaceAll(tmpl, "{{IS_NEW}}",       _isNewSession ? "true" : "false");

    _body = tmpl;
    _statusCode = OK;
    _headers["Content-Type"] = "text/html";
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

