This project has been created as part of the 42 curriculum by jrandet, hdougoud, chpasqui

# Description : presents the project, including its goal and a brief overview




# Instructions : relevant information about compilation, installation, and/or execution
For compiling the project make re, you can choose the level or log by making make re LOG_LEVEL=level number 
level number 1 = error
2 = above + warning
3 = above + infos 
4 = above + debug

We recommend using LOG_LEVEL=2 to see erros and warning when a request fails.

Once the program is compiled, select the configuration file to be set :
./webserv ./config/<configFile.conf>

Once the server is running you can send request through different ways, a selection of request has already been provided you can send them using by using another terminal and send :
nc localhost 8080 < tools/<select the request to be sent>

you may also use telnet localhost 8080 and send request manually

or curl (see how to use) or directly open a browser and try to reach different.


# Resources

## Books:
- Beej's Guide to Network Programming : https://beej.us/guide/bgnet/
- The Linux Programming Language : https://github.com/iceqp/books-1/blob/master/The%20Linux%20Programming%20Interface.pdf

## Server Loop:
- Socket Programming in C : https://www.geeksforgeeks.org/c/socket-programming-cc/
- Nonblocking I/O, Event-Driven Programming with epoll : https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/19-slides.pdf
- Non-Blocking Sockets and I/O Multiplexing with epoll in C : https://medium.com/@hajorda/non-blocking-sockets-and-i-o-multiplexing-with-epoll-in-c-bd3d8e54c20a
- epoll man : https://man7.org/linux/man-pages/man7/epoll.7.html
- difference between level-triggering and edge-triggering (EPOLLET) : https://medium.com/@m-ibrahim.research/mastering-epoll-the-engine-behind-high-performance-linux-networking-85a15e6bde90
- What is epoll ? : https://medium.com/@avocadi/what-is-epoll-9bbc74272f7c

## Http Parser Documentation:
- WebServ: Building a Non-Blocking Web Server in C++98 : https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7
- RFC HTTP 1.0 : https://www.rfc-editor.org/rfc/rfc1945
- RFC HTTP 1.1 : https://datatracker.ietf.org/doc/html/rfc7231

## Config File:
- Nginx configuration file beginner's guide : https://nginx.org/en/docs/beginners_guide.html

to see full documentation : https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing

## How AI was used ?
- challenge reflexion
- challenge different point of view
- review specific code
- create brief dyoxygen comment
- to make beautiful html pages
- help to write readme documentation