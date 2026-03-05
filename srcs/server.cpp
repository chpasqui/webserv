#include "../include/server.hpp"
#include "../include/main.hpp"
#include <string>
#include <errno.h>
#include <netinet/in.h>

int createSocket(void)
{
	int SocketFd;
	
	// Creating socket file descriptor
	SocketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (SocketFd == -1)
		perror("server: socket");
	
	int optval = 1;									// 1 is set to enable option
	socklen_t optlen = sizeof(optval);
	if (setsockopt(SocketFd, SOL_SOCKET,
		SO_REUSEADDR, &optval, optlen)) {
				
	}												// optlen = information for the setsockopt as optval is linked to SO_REUSEADDR
			
	// Forcefully attaching the socket to the port 8080 (PORT)
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET; 					// Address family, AF_INET
	my_addr.sin_port = htons(PORT);					// htons = host to network short, used to set it to Network Byte Order (Big Endian)
	my_addr.sin_addr.s_addr = INADDR_ANY; 			// TODO: check if this is valid
	
}
