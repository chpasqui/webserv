#include "../../include/ServerBlock.hpp"
#include <iostream>
#include "../../include/color.hpp"

ServerBlock::ServerBlock() : _dictionary({"http", "server", "root", "listen"}) {

}

ServerBlock::~ServerBlock() {
	std::cout << C_RED << "ServerBlock: Destructor called" << RESET << std::endl;
	return ;
}
