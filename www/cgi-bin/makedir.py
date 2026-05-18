#!/usr/bin/env python3
print("Content-Type: text/plain\r")
print("\r")
print("hello from CGI")

f = open("./test.txt", "w")   # 'r' for reading and 'w' for writing
f.write("Hello World from " + f.name)