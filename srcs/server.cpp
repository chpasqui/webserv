#include "../include/server.hpp"
#include "../include/main.hpp"
#include <string>
#include <errno.h>
#include <netinet/in.h>

int createSocket(void)
{
	int serverSocketFd;
	struct sockaddr_in my_addr;
	int optval = 1; 
	socklen_t optlen = sizeof(optval);

	serverSocketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocketFd == -1)
		perror("server: socket");
	if (setsockopt(serverSocketFd, SOL_SOCKET,
					SO_REUSEADDR, &optval, optlen)) // TODO: check this later a bit blurry
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY; // TODO: check if this is valid


}