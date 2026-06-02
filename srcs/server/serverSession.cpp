#include "../../include/Server.hpp"
#include "../../include/color.hpp"
#include "../../include/statusCode.hpp"
#include "../../include/Debug.hpp"

Session& Server::resolveSession(const std::string& cookieHeader, bool& isNew)
{
	std::string sessionId = Session::extractSessionId(cookieHeader);
	if (sessionId.empty() || _sessions.find(sessionId) == _sessions.end()) {
		sessionId = Session::generateId();
		_sessions[sessionId].setId(sessionId);
		isNew = true;
		LOG_INFO(C_YELLOW << "NEW session: " << sessionId);
    }
	else
			LOG_INFO(C_GREEN << "EXISTING session: " << sessionId);
	return (_sessions[sessionId]);
}
