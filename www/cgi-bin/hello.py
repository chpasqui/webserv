#!/usr/bin/env python3
import os
import sys

# read body from stdin if POST
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
body = sys.stdin.read(content_length) if content_length > 0 else ""

method = os.environ.get('REQUEST_METHOD', '')

# CGI response — headers first, blank line, then body
print("content-Length: " + str(content_length))
print("content-Type: text/html")
print("")  # blank line is mandatory
print("<html><body>")
print("<h1>Hello from CGI!</h1>")
print("<p>method: " + method + "</p>")
print("<p>body: " + body + "</p>")
print("</body></html>")