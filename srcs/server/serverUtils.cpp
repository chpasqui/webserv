#include "../include/Server.hpp"
#include "../include/color.hpp"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <string>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <csignal>
#include <signal.h>

void	Server::timeoutCheck(void) {
	time_t	currentTime;
	time(&currentTime);

	// timeoutCheckClient()
	// step1 : first loop like below
	// step2 : second loop of std::map<int, Client *> _pipeFd and check if the client = pipeFd in case a CGI is running
	// step3 : keep the if difftime like below but change the logic cause this->_client.erase(it->first) invalids the pointer on it (not c++11)
	// timeoutCheckCgi()
	// step1 : loop on pipeFd 
	// step2: check the diff time like below (need to see where to put the start time of cgi -> maybe launch ??)
	// step3: same logic as before however not forgetting to kill the child before
	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (std::difftime(currentTime, it->second->getTimestamp()) == TIMEOUT)
		{
			this->_clients.erase(it->first);
		}
	}
}

// SIGNAL HANDLING UTILS

void	Server::signalHandler(int signal) {
	if (signal == SIGINT)
	{
		
		std::cout << "\nSIGINT signal detected, stopping server\n";
	}
	else if (signal == SIGQUIT)
		std::cout << "\nSIGQUIT signal detected, stopping server\n";
	serverSignal = signal;
}


Session& Server::resolveSession(const std::string& cookieHeader, bool& isNew)
{
	std::string sessionId = Session::extractSessionId(cookieHeader);
	if (sessionId.empty() || _sessions.find(sessionId) == _sessions.end()) {
		sessionId = Session::generateId();
		_sessions[sessionId].setId(sessionId);
		isNew = true;
		std::cout << C_YELLOW << "NEW session: " << sessionId << RESET << std::endl;
    }
	else
		std::cout << C_GREEN << "EXISTING session: " << sessionId << RESET << std::endl;
	return (_sessions[sessionId]);
}