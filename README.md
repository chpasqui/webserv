# Sources
https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing


Notes to be adressed:
- to see but look on how to remove still reachable bytes other than calling the server destructor 

- finish the waitpid

- In Server.cpp - SingalHandler : I think we need to add signals for SIGPIPE (if the pipe is closed and we try to write on)
