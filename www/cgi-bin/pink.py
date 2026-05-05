#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

content_length = int(os.environ.get('CONTENT_LENGTH') or 0)
method = os.environ.get('REQUEST_METHOD', 'GET')
host = os.environ.get('HTTP_HOST', 'localhost')

raw = sys.stdin.read(content_length) if content_length > 0 else ""
data = parse_qs(raw)
name = data.get('name', [''])[0]

if name:
    greeting = f"OMG HI {name.upper()}!!! 🎀"
    sub = "you are literally so valid for visiting this website"
else:
    greeting = "welcome to the internet's pinkest website 🌸"
    sub = "no seriously. its very pink. you have been warned."

html = f"""<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>💖 pink zone 💖</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Pacifico&family=Nunito:wght@400;700;900&display=swap');

    * {{ margin: 0; padding: 0; box-sizing: border-box; }}

    body {{
      font-family: 'Nunito', sans-serif;
      background-color: #ffb6c1;
      background-image:
        radial-gradient(circle at 20% 20%, #ff69b4 0%, transparent 40%),
        radial-gradient(circle at 80% 80%, #ff1493 0%, transparent 40%);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      padding: 2rem;
      color: #7d0050;
    }}

    .sparkles {{ font-size: 2rem; letter-spacing: .5rem; margin-bottom: 1rem; animation: bounce 1s infinite alternate; }}
    @keyframes bounce {{ from {{ transform: translateY(0); }} to {{ transform: translateY(-10px); }} }}

    h1 {{
      font-family: 'Pacifico', cursive;
      font-size: 3rem;
      color: #fff;
      text-shadow: 3px 3px 0 #ff1493, 6px 6px 0 #c2005a;
      text-align: center;
      margin-bottom: .75rem;
    }}

    .sub {{
      font-size: 1.1rem;
      color: #fff;
      text-align: center;
      margin-bottom: 2.5rem;
      font-weight: 700;
    }}

    .card {{
      background: rgba(255,255,255,0.6);
      border: 3px solid #ff69b4;
      border-radius: 24px;
      padding: 2rem 2.5rem;
      max-width: 500px;
      width: 100%;
      text-align: center;
      backdrop-filter: blur(4px);
    }}

    .fact {{
      font-size: 1rem;
      line-height: 1.8;
      color: #7d0050;
      margin-bottom: 1.5rem;
    }}

    .fact b {{ color: #ff1493; }}

    form {{ display: flex; flex-direction: column; gap: .75rem; margin-top: 1rem; }}

    input[type=text] {{
      padding: .7rem 1rem;
      border: 2px solid #ff69b4;
      border-radius: 50px;
      font-family: 'Nunito', sans-serif;
      font-size: 1rem;
      color: #7d0050;
      background: #fff0f5;
      outline: none;
      text-align: center;
    }}

    input[type=submit] {{
      padding: .7rem 1rem;
      background: #ff1493;
      color: #fff;
      border: none;
      border-radius: 50px;
      font-family: 'Nunito', sans-serif;
      font-size: 1rem;
      font-weight: 900;
      cursor: pointer;
      letter-spacing: .05em;
    }}

    .footer {{
      margin-top: 2rem;
      font-size: .8rem;
      color: #fff;
      opacity: .7;
    }}
  </style>
</head>
<body>

  <div class="sparkles">✨🎀✨</div>
  <h1>{greeting}</h1>
  <p class="sub">{sub}</p>

  <div class="card">
    <p class="fact">
      did you know that <b>flamingos</b> are pink because they eat shrimp?
      this website is pink because <b>we said so.</b> 🦩
      <br><br>
      the server running this page has <b>absolutely no idea</b>
      it's this cute. it just forks a process and hopes for the best. 🍴
    </p>

    <form method="POST" action="/cgi-bin/pink.py">
      <input type="text" name="name" placeholder="what's your name bestie">
      <input type="submit" value="💖 say hi 💖">
    </form>
  </div>

  <p class="footer">
    powered by CGI/1.1 · handcrafted with zero regrets · host: {host}
  </p>

</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(html.encode('utf-8'))}")
print("")
print(html)