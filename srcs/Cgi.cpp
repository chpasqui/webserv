#include "Cgi.hpp"

// CGI will need to receive the request information as a pointer. from request, send the pointer this
// Cgi::Cgi() : 
// {
// 	// how do i pass on the originalEnv all the way to the cgi...?
// 	// intuitively lets pass it through server although not super clean...
// }

Cgi::Cgi(char **originalEnv) : _tempEnv(), _originalEnv(originalEnv), _finalEnv()
{
	
}

Cgi::~Cgi()
{

}
