#!/usr/bin/env python3
import os

rows = ""
for k, v in sorted(os.environ.items()):
    rows += f'<div class="row"><span class="key">{k}</span><span class="val">{v}</span></div>\n'

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>ENV — webserv CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{{box-sizing:border-box;margin:0;padding:0}}
    :root{{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a}}
    body{{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);padding:3rem 2rem}}
    .header{{max-width:760px;margin:0 auto 2rem}}
    .eyebrow{{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1rem}}
    h1{{font-family:'Playfair Display',serif;font-size:2.5rem;font-weight:700;line-height:.9}}
    h1 em{{font-style:italic;font-weight:400;color:var(--accent)}}
    .rows{{max-width:760px;margin:0 auto;display:flex;flex-direction:column}}
    .row{{display:flex;justify-content:space-between;padding:.7rem 0;border-bottom:1px solid var(--mid);font-size:.68rem;gap:1rem}}
    .row:first-child{{border-top:1px solid var(--mid)}}
    .key{{color:var(--soft);letter-spacing:.05em;flex-shrink:0}}
    .val{{color:var(--ink);text-align:right;word-break:break-all}}
    .back{{display:inline-block;margin-top:2rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}}
    .back:hover{{color:var(--accent)}}
  </style>
</head>
<body>
  <div class="header">
    <p class="eyebrow">CGI · python3</p>
    <h1>Variables <em>d'env.</em></h1>
  </div>
  <div class="rows">
    {rows}
  </div>
  <div style="max-width:760px;margin:0 auto">
    <a class="back" href="/">← accueil</a>
  </div>
</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(html.encode('utf-8'))}")
print()
print(html)
