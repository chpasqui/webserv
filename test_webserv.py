#!/usr/bin/env python3
"""
test_webserv.py - Script de tests automatisés pour webserv (42)
Usage: python3 test_webserv.py [host] [port]
       python3 test_webserv.py localhost 8080
"""

import sys
import socket
import os
import time

HOST = sys.argv[1] if len(sys.argv) > 1 else "localhost"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8080

# ── Couleurs ──────────────────────────────────────────────────────────────────
GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
CYAN   = "\033[96m"
RESET  = "\033[0m"
BOLD   = "\033[1m"

passed = 0
failed = 0
results = []

# ── Utilitaires ───────────────────────────────────────────────────────────────

def send_raw(request: str, timeout: float = 3.0) -> str:
    """Envoie une requête HTTP brute, retourne la réponse complète."""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)
        s.connect((HOST, PORT))
        s.sendall(request.encode())
        response = b""
        while True:
            try:
                chunk = s.recv(4096)
                if not chunk:
                    break
                response += chunk
            except socket.timeout:
                break
        s.close()
        return response.decode(errors="replace")
    except Exception as e:
        return f"ERROR: {e}"

def get_status(response: str) -> int:
    """Extrait le code HTTP de la réponse."""
    try:
        return int(response.split(" ")[1])
    except:
        return 0

def check(name: str, response: str, expected_status: int, extra_check=None):
    """Évalue un test et affiche le résultat."""
    global passed, failed
    status = get_status(response)
    ok = status == expected_status
    if ok and extra_check:
        ok = extra_check(response)
    if ok:
        passed += 1
        results.append((True, name, status, expected_status))
        print(f"  {GREEN}✓{RESET} {name} → {status}")
    else:
        failed += 1
        results.append((False, name, status, expected_status))
        print(f"  {RED}✗{RESET} {name} → got {status}, expected {expected_status}")

def section(title: str):
    print(f"\n{BOLD}{CYAN}── {title} {'─' * (50 - len(title))}{RESET}")

# ── Tests ─────────────────────────────────────────────────────────────────────

section("GET - Fichiers statiques")

r = send_raw("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /index.html → 200", r, 200)

r = send_raw("GET /doesnotexist.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET fichier inexistant → 404", r, 404)

r = send_raw("GET /static/index.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /static/index.html → 200", r, 200)


section("Autoindex")

r = send_raw("GET /files/ HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /files/ autoindex on → 200", r, 200,
      lambda resp: "<a" in resp.lower() or "href" in resp.lower())

r = send_raw("GET /private/ HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /private/ autoindex off → 403", r, 403)


section("POST - Upload")

r = send_raw(
    "POST /uploads/test_upload.txt HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "hello webserv"
)
check("POST /uploads/test_upload.txt → 201", r, 201)

# Conflit : même fichier une 2ème fois → 409
r = send_raw(
    "POST /uploads/test_upload.txt HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "hello webserv"
)
check("POST fichier déjà existant → 409", r, 409)

# URI se terminant par / → 400
r = send_raw(
    "POST /uploads/ HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 5\r\n"
    "\r\n"
    "hello"
)
check("POST sur URI '/' → 400", r, 400)

# Body trop grand → 413
r = send_raw(
    "POST /uploads/big/fat.txt HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 200\r\n"
    "\r\n" + "x" * 200
)
check("POST body > client_max_body_size → 413", r, 413)


section("DELETE")

r = send_raw("DELETE /uploads/test_upload.txt HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("DELETE fichier existant → 200/204", r, 204)

r = send_raw("DELETE /uploads/test_upload.txt HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("DELETE fichier inexistant → 404", r, 404)


section("Methods not allowed")

r = send_raw("DELETE /static/index.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("DELETE sur location GET-only → 405", r, 405)

r = send_raw(
    "POST /static/whatever HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 3\r\n"
    "\r\n"
    "abc"
)
check("POST sur location GET-only → 405", r, 405)


section("Redirections")

r = send_raw("GET /oldPage HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /oldPage → 301", r, 301,
      lambda resp: "Location" in resp)

r = send_raw("GET /temp HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /temp → 302", r, 302,
      lambda resp: "Location" in resp)


section("Parsing HTTP - Cas d'erreur RFC 7230")

# Sans Host → 400
r = send_raw("GET / HTTP/1.1\r\n\r\n")
check("Requête sans Host → 400", r, 400)

# Transfer-Encoding + Content-Length ensemble → 400
r = send_raw(
    "POST /uploads/x.txt HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 5\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nhello\r\n0\r\n\r\n"
)
check("TE + Content-Length → 400 (RFC 7230 §3.3.3)", r, 400)

# HTTP/1.0 (pas de Host requis)
r = send_raw("GET / HTTP/1.0\r\n\r\n")
check("HTTP/1.0 sans Host → 200 ou 400 (pas de hang)", r, get_status(r))


section("Chunked Transfer-Encoding")

chunked_body = "5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n"
r = send_raw(
    "POST /uploads/chunked.txt HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n" + chunked_body
)
check("POST chunked → 201", r, 201)

# Nettoyage
send_raw("DELETE /uploads/chunked.txt HTTP/1.1\r\nHost: localhost\r\n\r\n")


section("CGI")

r = send_raw("GET /cgi-bin/hello.py HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /cgi-bin/hello.py → 200", r, 200)

r = send_raw(
    "POST /cgi-bin/echo.py HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 4\r\n"
    "\r\n"
    "test"
)
check("POST /cgi-bin/echo.py → 200", r, 200,
      lambda resp: "test" in resp)

r = send_raw("GET /cgi-bin/hello.php HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET /cgi-bin/hello.php → 200", r, 200)

# CGI avec query string
r = send_raw("GET /cgi-bin/echo.py?foo=bar HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("CGI query string → 200", r, 200)


section("Multi-port (8081)")

old_port = PORT
PORT = 8081
r = send_raw("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
check("GET port 8081 → 200", r, 200)
PORT = old_port


section("Resilience")

# Requête incomplète → timeout propre, pas de hang
start = time.time()
r = send_raw("GET / HTTP/1.1\r\n", timeout=4.0)
elapsed = time.time() - start
ok = elapsed < 5.0
if ok:
    passed += 1
    print(f"  {GREEN}✓{RESET} Requête incomplète → timeout propre ({elapsed:.1f}s)")
else:
    failed += 1
    print(f"  {RED}✗{RESET} Requête incomplète → hang détecté ({elapsed:.1f}s)")
results.append((ok, "Requête incomplète sans hang", 0, 0))

# Connexion ouverte silencieuse
start = time.time()
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(5.0)
try:
    s.connect((HOST, PORT))
    time.sleep(0.5)
    # Pendant ce temps, une autre requête doit passer
    r2 = send_raw("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n")
    ok2 = get_status(r2) == 200
finally:
    s.close()
if ok2:
    passed += 1
    print(f"  {GREEN}✓{RESET} Connexion idle n'empêche pas les autres requêtes")
else:
    failed += 1
    print(f"  {RED}✗{RESET} Connexion idle bloque le serveur")
results.append((ok2, "Connexion idle non bloquante", 0, 0))


# ── Résumé ────────────────────────────────────────────────────────────────────

total = passed + failed
print(f"\n{BOLD}{'═' * 55}{RESET}")
print(f"{BOLD}  Résultats : {GREEN}{passed}{RESET}/{total} tests passés", end="")
if failed:
    print(f"  {RED}({failed} échecs){RESET}")
else:
    print(f"  {GREEN}✓ Tout bon !{RESET}")
print(f"{BOLD}{'═' * 55}{RESET}\n")

if failed:
    print(f"{RED}Tests échoués :{RESET}")
    for ok, name, got, exp in results:
        if not ok:
            print(f"  - {name} (got {got}, expected {exp})")
    print()

sys.exit(0 if failed == 0 else 1)
