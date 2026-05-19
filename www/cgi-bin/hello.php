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
