#!/usr/bin/php-cgi
<?php
echo "Content-Type: text/html\n\n"; // <-- blank line: this is what splitOutput is looking for
echo "<h1>Hello from PHP CGI</h1>";
echo "<p>SERVER_NAME: " . $_SERVER['SERVER_NAME'] . "</p>";
?>