# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- In executeScript : The CGI should be run in the correct directory for relative path file access (line from the subject) -> is it the case right now ?
- In executeScript : maybe change the parseSheBang with the use of the cgiPass (from class Response)
- In Server.cpp - SingalHandler : I think we need to add signals for SIGPIPE (if the pipe is closed and we try to write on it) and SIGCHLD (reapes automatically the dead child) therefore -> SIGCHLD can replace waitpid in readFromPipe (I think, need to be checked)
- In TimeOutCheck : the actual function is for incomplete request, there is an error on the it, and is not yet called anywhere. Need to complete the timeoutCheck functions for CGIs --- WORK IN PROGRESS
- In Server.cpp (read/write): There is a problem with the flow of modifyEpoll if it fails,it is missing the return false, and in case of failure the fd is closed 3 times (in modifyEpoll, in deleteEpoll and in closeConnections). Since it is already closed it will fail in deleteEpoll. Only closeConnections should close connections.
- In Session.cpp : maybe add with possibilty to add passwords or more, for more clarity. Need something that enables us to check in a better way whether it is a new or old sesion