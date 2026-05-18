# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- finish the waitpid and error codes in cgi 

- In Server.cpp - SingalHandler : I think we need to add signals for SIGPIPE (if the pipe is closed and we try to write on)

- In Session.cpp : maybe add with possibilty to add passwords or more, for more clarity. Need something that enables us to check in a better way whether it is a new or old session
- handle the open fd when ctrl C is pressed 

- Check if it is all good for : DeleteFromEpoll in ReadFromClient

- In Session.cpp : maybe add with possibilty to add passwords or more, for more clarity. Need something that enables us to check in a better way whether it is a new or old sesion - SIEGE regarder comment il fait (voir video et verifier)

- launch devrait etre bool pour return true ou false ?