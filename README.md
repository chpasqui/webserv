# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- The CGI should be run in the correct directory for relative path file access -> is it the case right now ?
- need to add signals for SIGPIPE and SIGCHLD -> SIGCHLD can replace waitpid in readFromPipe
- timeoutCheck is for incomplete request, there is an error on the it, is not yet called anywhere. Need to complete the timeoutCheck functions for CGIs
- Problems with the flow of modifyEpoll, missing return false, and in case of false the fd is closed 3 times (in modifyEpoll, in deleteEpoll and in closeConnections). It will fail in deleteEpoll since it has already been deleted in modifyEpoll. Only closeConnections should close connections