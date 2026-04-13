#include "Cgi.hpp"

// CGI will need to receive the request information as a pointer. from request, send the pointer this
Cgi::Cgi() : response(0), _env()
{
	// how do i pass on the env all the way to the cgi...?
	// intuitively lets pass it through server although not super clean...
}

Cgi::~Cgi()
{

}