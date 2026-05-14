# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- In executeScript : The CGI should be run in the correct directory for relative path file access (line from the subject) -> is it the case right now ?
- In executeScript : maybe change the parseSheBang with the use of the cgiPass (from class Response) DONE
- finish the waitpid and error codes in cgi 
- In Server.cpp - SingalHandler : I think we need to add signals for SIGPIPE (if the pipe is closed and we try to write on)
- In TimeOutCheck : the actual function is for incomplete request, there is an error on the it, and is not yet called anywhere. Need to complete the timeoutCheck functions for CGIs --- WORK IN PROGRESS
- In Session.cpp : maybe add with possibilty to add passwords or more, for more clarity. Need something that enables us to check in a better way whether it is a new or old session
- handle the open fd when ctrl C is pressed 
- For script.php : add SCRIPT_FILENAME in buildEnv
- Check if it is all good for : DeleteFromEpoll in ReadFromClient && addToEpoll from ReadFromPipe
- In Session.cpp : maybe add with possibilty to add passwords or more, for more clarity. Need something that enables us to check in a better way whether it is a new or old sesion - SIEGE regarder comment il fait (voir video et verifier)
- launch devrait etre bool pour return true ou false