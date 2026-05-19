This project has been created as part of the 42 curriculum by jrandet, hdougoud, chpasqui

# webserv

![C++98](https://img.shields.io/badge/C%2B%2B-98-blue) ![HTTP/1.1](https://img.shields.io/badge/HTTP-1.1-blue) ![epoll](https://img.shields.io/badge/I%2FO-epoll-green)

An HTTP/1.1 web server implemented in C++98, built as part of the 42 curriculum. The server handles concurrent connections via a non-blocking `epoll`-based event loop, parses and responds to HTTP requests, supports CGI execution, and reads its configuration from an nginx-inspired config file format.

## Features

- Non-blocking I/O with `epoll` (level-triggered)
- HTTP/1.1: GET, POST, DELETE — including chunked transfer encoding
- CGI support (Python, PHP) via `fork`/`exec` with pipe-based I/O
- Nginx-style config: virtual hosts, location blocks, redirects, autoindex
- File uploads
- Cookie-based session management
- Configurable logging levels

## Compilation

```bash
make re
make re LOG_LEVEL=2   # recommended: errors + warnings
```

| LOG_LEVEL | Output |
|-----------|--------|
| `1` | Errors only |
| `2` | Errors + warnings *(recommended)* |
| `3` | + informational messages |
| `4` | + debug output |

## Usage

```bash
./webserv ./config/<configFile.conf>
```

### Sending requests

Several pre-built raw requests are available in `tools/`:

```bash
nc localhost 8080 < tools/<request_file>
```

Or interactively:

```bash
telnet localhost 8080
```

Or with curl:

```bash
curl -v http://localhost:8080/
curl -v -X POST -d "key=value" http://localhost:8080/upload
curl -v -X DELETE http://localhost:8080/file.txt
```

You can also open a browser and navigate to `http://localhost:8080` directly.

> The default port is 8080, but it depends on your config file. Check your `.conf` to confirm which ports and virtual hosts are active.

## Configuration

Config files follow an nginx-inspired syntax, wrapped in an `http { }` block. Multiple `server` blocks are supported. See `config/` for full examples.

Key directives:

- `listen` — port to bind
- `server_name` — virtual host name
- `root` — document root
- `index` — default file to serve
- `client_max_body_size` — request body size limit (overridable per location)
- `client_timeout` — connection timeout in seconds
- `error_page <code> <path>` — custom error pages
- `location` blocks with `allowed_methods`, `autoindex`, `index`, `root`
- `return <code> <url>` — redirects (301, 302)
- `cgi_pass .ext /path/to/interpreter` — per-extension CGI handler

Example:

```nginx
http {
  server {
    listen 8080
    server_name localhost
    root www/
    client_max_body_size 10000
    error_page 404 /errors/404.html

    location / {
      allowed_methods "GET|POST|DELETE"
      autoindex on
    }
    location /uploads/ {
      root www/uploads/
      allowed_methods "GET|POST|DELETE"
      autoindex on
      client_max_body_size 5000
    }
    location /cgi-bin {
      allowed_methods "GET|POST"
      root www/cgi-bin/
      cgi_pass .py  /usr/bin/python3
      cgi_pass .php /usr/bin/php-cgi
    }
    location /oldPage {
      return 301 /newPage
    }
  }
}
```

## Resources

### Books

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [The Linux Programming Interface](https://github.com/iceqp/books-1/blob/master/The%20Linux%20Programming%20Interface.pdf)

### Server loop & epoll

- [Socket Programming in C](https://www.geeksforgeeks.org/c/socket-programming-cc/)
- [Nonblocking I/O & Event-Driven Programming with epoll](https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/19-slides.pdf) — Stanford CS110
- [Non-Blocking Sockets and epoll in C](https://medium.com/@hajorda/non-blocking-sockets-and-i-o-multiplexing-with-epoll-in-c-bd3d8e54c20a)
- [epoll(7) man page](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [Level vs edge triggering (EPOLLET)](https://medium.com/@m-ibrahim.research/mastering-epoll-the-engine-behind-high-performance-linux-networking-85a15e6bde90)
- [What is epoll?](https://medium.com/@avocadi/what-is-epoll-9bbc74272f7c)

### HTTP

- [WebServ: Building a Non-Blocking Web Server in C++98](https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7)
- [RFC 1945 — HTTP/1.0](https://www.rfc-editor.org/rfc/rfc1945)
- [RFC 7231 — HTTP/1.1 Semantics](https://datatracker.ietf.org/doc/html/rfc7231)

### Configuration

- [Nginx config beginner's guide](https://nginx.org/en/docs/beginners_guide.html)
- [Full directive documentation (Google Sheets)](https://docs.google.com/spreadsheets/d/1VZfzkXoYSBPIKUkYGd2LAbqP2WufRq0hg4a3hBmlzUE/edit?usp=sharing)

## Use of AI

AI tools were used during development for:

- Challenging design decisions and exploring alternative approaches
- Reviewing specific code sections
- Generating Doxygen comments
- Building HTML pages for the server
- Writing parts of this documentation