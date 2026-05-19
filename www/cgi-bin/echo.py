#!/usr/bin/env python3
import sys
import os

content_length = int(os.environ.get('CONTENT_LENGTH') or 0)
body = sys.stdin.read(content_length) if content_length > 0 else ""

print("Content-Type: text/plain")
print(f"Content-Length: {len(body.encode('utf-8'))}")
print()
print(body)
