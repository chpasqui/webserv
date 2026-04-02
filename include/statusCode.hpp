#ifndef STATUSCODE_HPP
# define STATUSCODE_HPP

typedef enum e_statusCode {
	OK = 200,
	BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
	URL_TOOLONG = 414,
	NOT_IMPLEMENTED = 501,
	WRONG_HTTP = 505,
} statusCode;


#endif