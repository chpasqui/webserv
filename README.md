# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- finish the waitpid and error codes in cgi 

- In Server.cpp - SingalHandler : I think we need to add signals for SIGPIPE (if the pipe is closed and we try to write on)

- Check if it is all good for : DeleteFromEpoll in ReadFromClient
