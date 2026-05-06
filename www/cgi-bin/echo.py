#!/usr/bin/env python3
import sys
import os

body = sys.stdin.read()
length = len(body)

print("Content-Type: text/plain")
print("Content-Length: " + str(length))
print("")
print(body)