#include "../include/utils.hpp"

#include <sstream>

std::string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	std::string newString;
	ss >> newString;
	return newString;
}