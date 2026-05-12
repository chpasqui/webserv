#ifndef DEBUG_HPP
# define DEBUG_HPP

# include <iostream>

# ifdef DEBUG
#  define LOG(msg) std::cerr << msg << std::endl
#  define LOG_ERR(code, ctx) std::cerr << "Error " << code << " : " << ctx << std::endl
# else
#  define LOG(msg) (void)0
#  define LOG_ERR(code, ctx) (void)0
# endif

#endif