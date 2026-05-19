#!/usr/bin/env python3
import os

cookie_header = os.environ.get('HTTP_COOKIE', '')
session_id = ''
cgi_visited = ''
for part in cookie_header.split(';'):
    part = part.strip()
    if part.startswith('session_id='):
        session_id = part[len('session_id='):]
    if part.startswith('cgi_visited='):
        cgi_visited = part[len('cgi_visited='):]

already = cgi_visited == 'true'

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Cookie Demo — webserv CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{{box-sizing:border-box;margin:0;padding:0}}
    :root{{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a;--green:#2d6a4f;--green-lt:#d8f3dc}}
    body{{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);min-height:100vh;display:grid;place-items:center;padding:2rem}}
    .card{{max-width:560px;width:100%;border:1px solid var(--mid);padding:3rem}}
    .eyebrow{{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1.5rem}}
    h1{{font-family:'Playfair Display',serif;font-size:2.8rem;font-weight:700;line-height:.9;margin-bottom:2rem}}
    h1 em{{display:block;font-style:italic;font-weight:400;color:var(--accent);font-size:.8em}}
    .banner{{font-size:.7rem;padding:.6rem 1rem;border-left:2px solid var(--accent);background:rgba(196,87,26,.08);color:var(--accent);margin-bottom:1.5rem}}
    .banner.ok{{border-left-color:var(--green);background:var(--green-lt);color:var(--green)}}
    .rows{{display:flex;flex-direction:column}}
    .row{{display:flex;justify-content:space-between;padding:.75rem 0;border-bottom:1px solid var(--mid);font-size:.7rem}}
    .row:first-child{{border-top:1px solid var(--mid)}}
    .key{{color:var(--soft);letter-spacing:.08em}}
    .val{{color:var(--accent)}}
    .back{{display:inline-block;margin-top:2rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}}
    .back:hover{{color:var(--accent)}}
  </style>
</head>
<body>
  <div class="card">
    <p class="eyebrow">CGI · cookie demo</p>
    <h1>Cookies<em>& session.</em></h1>
    {'<div class="banner ok">Déjà visité — cookie cgi_visited détecté.</div>' if already else '<div class="banner">Première visite — cookie cgi_visited en cours de création.</div>'}
    <div class="rows">
      <div class="row"><span class="key">session_id (serveur)</span><span class="val">{session_id or '(aucun)'}</span></div>
      <div class="row"><span class="key">cgi_visited (CGI)</span><span class="val">{cgi_visited or '(aucun)'}</span></div>
      <div class="row"><span class="key">HTTP_COOKIE complet</span><span class="val">{cookie_header or '(vide)'}</span></div>
    </div>
    <a class="back" href="/">← accueil</a>
  </div>
</body>
</html>"""

print("Set-Cookie: cgi_visited=true; Path=/")
print("Content-Type: text/html")
print(f"Content-Length: {len(html.encode('utf-8'))}")
print()
print(html)
