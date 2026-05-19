#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

content_length = int(os.environ.get('CONTENT_LENGTH') or 0)
host   = os.environ.get('HTTP_HOST', 'localhost')

raw  = sys.stdin.read(content_length) if content_length > 0 else ""
data = parse_qs(raw)
name = data.get('name', [''])[0]

if name:
    greeting = f"OMG HI {name.upper()}!!!"
    sub = "you are literally so valid for visiting this website"
else:
    greeting = "welcome to the internet's pinkest website"
    sub = "no seriously. its very pink. you have been warned."

html = f"""<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>pink zone</title>
  <style>
    body{{font-family:sans-serif;background:#ffb6c1;min-height:100vh;display:flex;flex-direction:column;align-items:center;justify-content:center;color:#7d0050}}
    h1{{font-size:2.5rem;color:#fff;text-shadow:3px 3px 0 #ff1493}}
    form{{display:flex;flex-direction:column;gap:.75rem;margin-top:1.5rem}}
    input{{padding:.6rem 1rem;border:2px solid #ff69b4;border-radius:50px;font-size:1rem;text-align:center}}
    input[type=submit]{{background:#ff1493;color:#fff;border:none;cursor:pointer;font-weight:bold}}
  </style>
</head>
<body>
  <h1>{greeting}</h1>
  <p>{sub}</p>
  <form method="POST" action="/cgi-bin/pink.py">
    <input type="text" name="name" placeholder="what's your name bestie">
    <input type="submit" value="say hi">
  </form>
  <p style="margin-top:2rem;font-size:.8rem;opacity:.7">host: {host}</p>
</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(html.encode('utf-8'))}")
print()
print(html)
