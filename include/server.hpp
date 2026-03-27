#ifndef SERVER_HPP
# define SERVER_HPP

# define PORT 8080
# define ROOT ./www/
# define BACKLOG 5
# define MAX_EVENT 5

#include "Request.hpp" //remove this later

#include <sys/types.h>
#include <sys/socket.h>

int		createListeningSocket(void);
int		bindListeningSocket(int *listenFd);
int		handleClientsConnections(int listenFd);

#endif
