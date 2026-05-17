#include "../../include/Response.hpp"
#include "../../include/ServerConfig.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Location.hpp"
#include "../../include/Debug.hpp"

#include <fstream>
#include <ctime>


void Response::serverSessionPage(){
    std::ostringstream page;
    double sessionTime = std::difftime(std::time(NULL), _session->getCreatedAt());

	page << "<html><body>";

    if (_isNewSession)
        page << "<p style='color:blue'>New session created: "<< _session->getId() << "</p>";
    else
        page << "<p style='color:green'>Welcome back - session: "<< _session->getId() 
        << " active since " << sessionTime << "s</p>";

    if (!_session->data["user"].empty())
        page << "<h1>Hey " << _session->data["user"] << "!</h1>\n";
	else 
		page << "<h1>Hey new visitor !</h1>" << "<a href='/login.html'>Login</a>";

	page << "</body></html>";
	_body = page.str();
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
    _headers["Location"] = "/welcome";
}