#include "../include/main.hpp"
#include "../include/server.hpp"

int main(int ac, char **av)
{
	(void) ac;
	(void) av;
//	if (ac > 2)
//		return (1);
	if (createSocket() == 1)
		return (1);
	return (0);
}
