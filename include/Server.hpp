#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <set>
#include <stdint.h>

#include "ServerConfig.hpp"
#include "Client.hpp"
#include "Session.hpp"

extern int serverSignal;
extern void safeClose(int fd);

// Client lives in server
// ServerConfig lives in Client
// Server lives in main
// server outlives serverconfig so it can have a pointer to serverconfig and it will delete it when closing

#define MAX_EVENTS 1024
#define TIMEOUT 10
#define BACKLOG 20

class Server
{
public:
	Server();
	void setupListeningSockets(const std::vector<ServerConfig *> &serverConfigsVec);
	~Server();
	void createEpollInstance(void);
	int getEpollFd(void) const;
	int openListeningSocket(void);
	void bindAndListen(int &fd, int port) const;
	void run(void);
	bool acceptNewConnection(int listeningFd);
	void readFromClient(int fd);
	void readFromPipe(int stdoutPipeReadFd);
	void launchCgi(Client &client);
	void writeToClient(int fd);
	void writeToPipe(Client &client, int stdinPipeWriteFd);
	void timeoutCheck(void);

	bool cgiDead(int fd);

	void closeConnection(int fd);
	static void addToEpoll(int epollFd, int fdToModify, uint32_t flag);
	static bool modifyEpoll(int epollFd, int fdToModify, uint32_t flag);
	static void delFromEpoll(int epollFd, int fdToModify);
	static void signalHandler(int signal);
	void updateTimeStamp(Client &client);

private:
	int _epollFd;
	std::map<int, const ServerConfig *> _listenFds; // if it wasnt a pointer, server would own another copy of the serverconfig which is wasteful
	std::map<int, Client *> _clients;
	std::map<std::string, Session> _sessions;
	std::map<int, Client *> _pipeFd;

	Session &resolveSession(const std::string &cookieHeader, bool &isNew);
};

#endif



