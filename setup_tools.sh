#!/bin/bash
# setup_tools.sh - Crée les scripts CGI et les requêtes nc
# Usage: bash setup_tools.sh (depuis la racine du projet)

set -e

mkdir -p www/cgi-bin
mkdir -p tools/requests

# ─────────────────────────────────────────────────────────────────────────────
# PYTHON SCRIPTS
# ─────────────────────────────────────────────────────────────────────────────

# hello.py — GET : page d'accueil soignée
cat > www/cgi-bin/hello.py << 'PYEOF'
#!/usr/bin/env python3
import os

server_name = os.environ.get('SERVER_NAME', 'unknown')
server_port = os.environ.get('SERVER_PORT', 'unknown')
method      = os.environ.get('REQUEST_METHOD', 'unknown')
query       = os.environ.get('QUERY_STRING', '')

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Hello — webserv CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{{box-sizing:border-box;margin:0;padding:0}}
    :root{{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a}}
    body{{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);min-height:100vh;display:grid;place-items:center;padding:2rem}}
    .card{{max-width:560px;width:100%;border:1px solid var(--mid);padding:3rem}}
    .eyebrow{{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1.5rem}}
    h1{{font-family:'Playfair Display',serif;font-size:3rem;font-weight:700;line-height:.9;margin-bottom:2rem}}
    h1 em{{display:block;font-style:italic;font-weight:400;color:var(--accent);font-size:.8em}}
    .rows{{display:flex;flex-direction:column;gap:0;margin-top:1.5rem}}
    .row{{display:flex;justify-content:space-between;padding:.75rem 0;border-bottom:1px solid var(--mid);font-size:.7rem}}
    .row:first-child{{border-top:1px solid var(--mid)}}
    .row .key{{color:var(--soft);letter-spacing:.08em}}
    .row .val{{color:var(--accent)}}
    .back{{display:inline-block;margin-top:2rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}}
    .back:hover{{color:var(--accent)}}
  </style>
</head>
<body>
  <div class="card">
    <p class="eyebrow">CGI · python3</p>
    <h1>Hello<em>world.</em></h1>
    <div class="rows">
      <div class="row"><span class="key">SERVER_NAME</span><span class="val">{server_name}</span></div>
      <div class="row"><span class="key">SERVER_PORT</span><span class="val">{server_port}</span></div>
      <div class="row"><span class="key">REQUEST_METHOD</span><span class="val">{method}</span></div>
      <div class="row"><span class="key">QUERY_STRING</span><span class="val">{query or '(empty)'}</span></div>
    </div>
    <a class="back" href="/">← accueil</a>
  </div>
</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(html.encode('utf-8'))}")
print()
print(html)
PYEOF

# echo.py — POST : renvoie le body reçu
cat > www/cgi-bin/echo.py << 'PYEOF'
#!/usr/bin/env python3
import sys
import os

content_length = int(os.environ.get('CONTENT_LENGTH') or 0)
body = sys.stdin.read(content_length) if content_length > 0 else ""

print("Content-Type: text/plain")
print(f"Content-Length: {len(body.encode('utf-8'))}")
print()
print(body)
PYEOF

# env.py — GET : dump toutes les variables d'environnement CGI, page soignée
cat > www/cgi-bin/env.py << 'PYEOF'
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
PYEOF

# cookie.py — GET : lit le cookie de session, set un cookie applicatif, page soignée
cat > www/cgi-bin/cookie.py << 'PYEOF'
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
PYEOF

# pink.py — POST/GET : page HTML fun avec formulaire
cat > www/cgi-bin/pink.py << 'PYEOF'
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
PYEOF

# infinite.py — GET : boucle infinie pour tester le timeout CGI
cat > www/cgi-bin/infinite.py << 'PYEOF'
#!/usr/bin/env python3
import time
import sys

print("Content-Type: text/html")
print()
sys.stdout.flush()
while True:
    time.sleep(1)
PYEOF

chmod +x www/cgi-bin/*.py
echo "✓ Scripts Python créés dans www/cgi-bin/"

# ─────────────────────────────────────────────────────────────────────────────
# PHP SCRIPTS
# ─────────────────────────────────────────────────────────────────────────────

# hello.php — GET : affiche variables serveur, page soignée
cat > www/cgi-bin/hello.php << 'PHPEOF'
<?php
$server_name = $_SERVER['SERVER_NAME'] ?? 'unknown';
$server_port = $_SERVER['SERVER_PORT'] ?? 'unknown';
$method      = $_SERVER['REQUEST_METHOD'] ?? 'unknown';
$query       = $_SERVER['QUERY_STRING'] ?? '';

$html = <<<HTML
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Hello — webserv PHP CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
    :root{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a}
    body{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);min-height:100vh;display:grid;place-items:center;padding:2rem}
    .card{max-width:560px;width:100%;border:1px solid var(--mid);padding:3rem}
    .eyebrow{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1.5rem}
    h1{font-family:'Playfair Display',serif;font-size:3rem;font-weight:700;line-height:.9;margin-bottom:2rem}
    h1 em{display:block;font-style:italic;font-weight:400;color:var(--accent);font-size:.8em}
    .badge{display:inline-block;font-size:.6rem;letter-spacing:.15em;text-transform:uppercase;padding:.25rem .65rem;border:1px solid var(--accent);color:var(--accent);margin-bottom:1.5rem}
    .rows{display:flex;flex-direction:column}
    .row{display:flex;justify-content:space-between;padding:.75rem 0;border-bottom:1px solid var(--mid);font-size:.7rem}
    .row:first-child{border-top:1px solid var(--mid)}
    .key{color:var(--soft);letter-spacing:.08em}
    .val{color:var(--accent)}
    .back{display:inline-block;margin-top:2rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}
    .back:hover{color:var(--accent)}
  </style>
</head>
<body>
  <div class="card">
    <p class="eyebrow">CGI · php-cgi</p>
    <h1>Hello<em>from PHP.</em></h1>
    <span class="badge">php-cgi</span>
    <div class="rows">
      <div class="row"><span class="key">SERVER_NAME</span><span class="val">{$server_name}</span></div>
      <div class="row"><span class="key">SERVER_PORT</span><span class="val">{$server_port}</span></div>
      <div class="row"><span class="key">REQUEST_METHOD</span><span class="val">{$method}</span></div>
      <div class="row"><span class="key">QUERY_STRING</span><span class="val">{$query}</span></div>
      <div class="row"><span class="key">PHP_VERSION</span><span class="val">PHP_VERSION_PLACEHOLDER</span></div>
    </div>
    <a class="back" href="/">← accueil</a>
  </div>
</body>
</html>
HTML;

$php_version = phpversion();
$html = str_replace('PHP_VERSION_PLACEHOLDER', $php_version, $html);

header("Content-Type: text/html");
header("Content-Length: " . strlen($html));
echo $html;
?>
PHPEOF

# form.php — POST/GET : formulaire avec page de résultat soignée
cat > www/cgi-bin/form.php << 'PHPEOF'
<?php
$method = $_SERVER['REQUEST_METHOD'] ?? 'GET';
$host   = $_SERVER['HTTP_HOST'] ?? 'localhost';

$name    = '';
$message = '';

if ($method === 'POST') {
    $raw  = file_get_contents('php://stdin');
    parse_str($raw, $data);
    $name    = htmlspecialchars($data['name']    ?? '');
    $message = htmlspecialchars($data['message'] ?? '');
}

if ($name && $message):
$html = <<<HTML
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Merci — webserv PHP CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
    :root{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a}
    body{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);min-height:100vh;display:grid;place-items:center;padding:2rem}
    .card{max-width:560px;width:100%;border:1px solid var(--mid);padding:3rem}
    .eyebrow{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1.5rem}
    h1{font-family:'Playfair Display',serif;font-size:2.8rem;font-weight:700;line-height:.9;margin-bottom:2rem}
    h1 em{display:block;font-style:italic;font-weight:400;color:var(--accent);font-size:.8em}
    .msg{font-size:.8rem;line-height:1.8;color:var(--soft);border-left:2px solid var(--accent);padding:.75rem 1rem;background:rgba(196,87,26,.05);margin-bottom:1.5rem}
    .back{display:inline-block;margin-top:1rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}
    .back:hover{color:var(--accent)}
  </style>
</head>
<body>
  <div class="card">
    <p class="eyebrow">CGI · php-cgi · réponse</p>
    <h1>Merci,<em>{$name}.</em></h1>
    <div class="msg">{$message}</div>
    <a class="back" href="/cgi-bin/form.php">← retour au formulaire</a>
  </div>
</body>
</html>
HTML;

else:
$html = <<<HTML
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Formulaire — webserv PHP CGI</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,700;1,400&family=DM+Mono:wght@300;400&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
    :root{--ink:#1a1a18;--soft:#5a5a52;--paper:#f5f2ea;--mid:#ede9de;--accent:#c4571a}
    body{font-family:'DM Mono',monospace;background:var(--paper);color:var(--ink);min-height:100vh;display:grid;place-items:center;padding:2rem}
    .card{max-width:560px;width:100%;border:1px solid var(--mid);padding:3rem}
    .eyebrow{font-size:.62rem;letter-spacing:.2em;text-transform:uppercase;color:var(--soft);margin-bottom:1.5rem}
    h1{font-family:'Playfair Display',serif;font-size:2.8rem;font-weight:700;line-height:.9;margin-bottom:2rem}
    h1 em{display:block;font-style:italic;font-weight:400;color:var(--accent);font-size:.8em}
    .field{display:flex;flex-direction:column;gap:.4rem;margin-bottom:1.5rem}
    label{font-size:.62rem;letter-spacing:.15em;text-transform:uppercase;color:var(--soft)}
    input,textarea{background:transparent;border:none;border-bottom:1px solid var(--mid);padding:.6rem 0;font-family:'DM Mono',monospace;font-size:.85rem;color:var(--ink);outline:none;transition:border-color .2s;width:100%}
    input:focus,textarea:focus{border-bottom-color:var(--accent)}
    textarea{resize:vertical;min-height:80px}
    button{background:var(--ink);color:var(--paper);border:none;font-family:'DM Mono',monospace;font-size:.7rem;letter-spacing:.15em;text-transform:uppercase;padding:.75rem 2rem;cursor:pointer;margin-top:.5rem;transition:background .2s}
    button:hover{background:var(--accent)}
    .back{display:inline-block;margin-top:2rem;font-size:.65rem;letter-spacing:.12em;text-transform:uppercase;color:var(--soft);text-decoration:none}
    .back:hover{color:var(--accent)}
  </style>
</head>
<body>
  <div class="card">
    <p class="eyebrow">CGI · php-cgi</p>
    <h1>Un<em>formulaire.</em></h1>
    <form method="POST" action="/cgi-bin/form.php">
      <div class="field">
        <label for="name">votre nom</label>
        <input type="text" id="name" name="name" placeholder="Alice…">
      </div>
      <div class="field">
        <label for="message">votre message</label>
        <textarea id="message" name="message" placeholder="Bonjour le monde…"></textarea>
      </div>
      <button type="submit">Envoyer →</button>
    </form>
    <a class="back" href="/">← accueil</a>
  </div>
</body>
</html>
HTML;

endif;

header("Content-Type: text/html");
header("Content-Length: " . strlen($html));
echo $html;
?>
PHPEOF

echo "✓ Scripts PHP créés dans www/cgi-bin/"

# ─────────────────────────────────────────────────────────────────────────────
# REQUETES NC (CRLF correct)
# ─────────────────────────────────────────────────────────────────────────────

printf "GET /cgi-bin/hello.py HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_hello_py.txt

printf "GET /cgi-bin/hello.php HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_hello_php.txt

printf "GET /cgi-bin/form.php HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_form_php.txt

printf "GET /cgi-bin/env.py HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_env.txt

printf "GET /cgi-bin/cookie.py HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_cookie.txt

printf "GET /cgi-bin/infinite.py HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_infinite.txt

printf "GET /files/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/get_autoindex.txt

printf "POST /cgi-bin/echo.py HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 22\r\nConnection: close\r\n\r\nHello My name is Alice" \
    > tools/requests/post_echo.txt

printf "POST /cgi-bin/pink.py HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 10\r\nConnection: close\r\n\r\nname=Alice" \
    > tools/requests/post_pink.txt

FORM_BODY="name=Alice&message=Bonjour+le+monde"
FORM_LEN=${#FORM_BODY}
printf "POST /cgi-bin/form.php HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ${FORM_LEN}\r\nConnection: close\r\n\r\n%s" \
    "${FORM_BODY}" \
    > tools/requests/post_form_php.txt
    
printf "POST /uploads/test.txt HTTP/1.1\r\nHost: localhost\r\nContent-Length: 13\r\nConnection: close\r\n\r\nhello webserv" \
    > tools/requests/post_upload.txt

printf "DELETE /uploads/test.txt HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" \
    > tools/requests/delete_upload.txt

printf "POST /uploads/chunked.txt HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n" \
    > tools/requests/post_chunked.txt

echo "✓ Requêtes nc créées dans tools/requests/"
echo ""
echo "Usage :"
echo "  nc localhost 8080 < tools/requests/get_hello_py.txt"
echo "  nc localhost 8080 < tools/requests/get_hello_php.txt"
echo "  nc localhost 8080 < tools/requests/get_form_php.txt"
echo "  nc localhost 8080 < tools/requests/get_cookie.txt"
echo "  nc localhost 8080 < tools/requests/post_form_php.txt"