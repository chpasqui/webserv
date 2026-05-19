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
