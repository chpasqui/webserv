# Fichiers de stress / crash pour le parsing de config webserv

Déposer ces `.conf` dans `tools/tester/test_files/` et les passer un par un à `./webserv <fichier>`.
Les noms encodent le comportement **attendu** : `crash_*` doit planter (bug à corriger),
`bug_*` révèle un comportement incorrect mais sans crash, `edge_*` doit être géré proprement,
`stress_*` teste la robustesse en volume.

## A. SEGFAULT garanti — la faille principale

Cause racine : dans le constructeur, `_currentContext(0)`. Aucune fonction `processXXXToken`
ne vérifie que `_currentContext != NULL` avant d'appeler `_currentContext->setXXX()`.
Donc **toute directive placée hors de `http{}` / `server{}` / `location{}`** déréférence un
pointeur nul → SIGSEGV (return code -11 sous Linux).

| Fichier | Contenu | Appel fautif |
|---|---|---|
| crash_01_listen_no_context.conf | `listen 8080` | `processListenToken` → `_currentContext->setPort()` |
| crash_02_root_no_context.conf | `root /var/www` | `processRootToken` → `setRoot()` |
| crash_03_autoindex_no_context.conf | `autoindex on` | `processAutoindexToken` → `setAutoindex()` |
| crash_04_cgi_no_context.conf | `cgi_pass .py ...` | `processCgiPassToken` → `addCgiExtension()` |
| crash_05_return_no_context.conf | `return 301 /redirect` | `processRedirectionToken` → `setRedireCode()` |
| crash_06_index_no_context.conf | `index index.html` | `processIndexToken` → `addIndex()` |
| crash_07_errorpage_no_context.conf | `error_page 404 /404.html` | `processErrorPageToken` → `addErrorPage()` |
| crash_08_listen_after_http_closed.conf | `http{}` puis `listen 8080` | `_currentContext` remis implicitement, niveau NONE |

**Correctif suggéré** : au début de chaque `processXXXToken` de directive, vérifier
`if (!_currentContext) throw std::runtime_error("\"<directive>\" hors de tout contexte");`
ou vérifier `_contextLevel` (chaque directive a un contexte autorisé documenté en commentaire).

## B. Bugs logiques — fichier cassé mais ACCEPTÉ (exit 0)

| Fichier | Problème |
|---|---|
| bug_09_missing_closing_brace.conf | Accolades jamais fermées. Comme 1 server a été poussé, l'`EndOfConfigFile` est interprété comme « No errors » → exit 0. Un fichier syntaxiquement invalide est validé. |
| bug_10_incomplete_directive_eof.conf | `listen` sans valeur, EOF atteint pendant `getNextToken()`. L'exception `EndOfConfigFile` remonte et est traitée comme un succès. Directive tronquée acceptée. |

**Cause** : `getNextToken()` lève `EndOfConfigFile` dès qu'une ligne manque, et ce signal
EOF sert à la fois de « fin normale » et de « fin inattendue au milieu d'une directive ».
Il faudrait suivre l'état (contexte encore ouvert ? directive en cours ?) au moment de l'EOF.

## C. Gestion des commentaires

| Fichier | Problème |
|---|---|
| bug_11_hash_no_space_drops_line.conf | `listen 8080#commentaire` (pas d'espace avant `#`). `find(" #")` ne matche pas, mais la boucle de `getNextLine()` voit `find("#") != npos` → relit la ligne suivante en **écrasant** `_currentLine`. La directive `listen 8080` est **perdue silencieusement**. |

## D. Parsing trop laxiste (pas de crash, mais aucune validation)

| Fichier | Problème |
|---|---|
| bug_12_number_trailing_garbage.conf | `listen 8080xyz` → `strToInt` lit `8080`, `ss.fail()` reste faux, la suite `xyz` est ignorée. Aucune erreur. |
| bug_13_semicolon_tolerated.conf | `listen 8080;` → le `;` est avalé dans le token et ignoré. La grammaire ne valide pas la fin de directive. |

(À toi de voir si c'est voulu — selon le sujet 42, ces cas devraient souvent être rejetés.)

## E. Edge cases — doivent être gérés SANS crash ni hang (non-régression)

| Fichier | Attendu |
|---|---|
| edge_14_blank_lines_spam.conf | 500 lignes vides dans `http{}` → `*_currentLine.begin()` sur string vide est techniquement UB ; vérifier qu'il n'y a pas de crash et que le parse réussit. |
| edge_15_only_comments.conf | Que des commentaires → doit finir en erreur « no server block », exit 1, propre. |
| edge_16_extra_closing_brace.conf | `}` seul au niveau NONE → `default:` du switch → throw géré, exit 1. |
| edge_17_tabs_and_crlf.conf | Fins de ligne CRLF (`\r\n`) → le `\r` doit être traité comme whitespace, exit 0. |

## F. Stress / volume (robustesse, fuites, temps)

| Fichier | Test |
|---|---|
| stress_18_many_servers.conf | 5000 blocs `server` → mémoire + temps, pas de fuite (lancer sous valgrind). |
| stress_19_many_locations.conf | 10000 `location` dans un server → `findLocation` est O(n), test de charge. |
| stress_20_huge_token.conf | Un token `root` de 8 Mo sur une seule ligne → robustesse de `getNextToken`/`substr`. |
| stress_21_one_long_line.conf | Tout sur une seule ligne géante (~1.4 Mo, 100000 tokens) → la logique ligne-par-ligne tient-elle ? |

## Conseil d'exécution

Pour les `crash_*` et `bug_*`, le serveur doit **s'arrêter** : utilise un `wait(timeout=...)`.
Pour vérifier le segfault : `proc.returncode == -11` (SIGSEGV). Lance aussi sous valgrind
pour les `stress_*` afin de détecter les fuites avant que ça ne « passe » par chance.
