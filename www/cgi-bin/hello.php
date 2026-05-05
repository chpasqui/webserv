#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");
echo "<h1>Hello from PHP CGI</h1>";
echo "<p>SERVER_NAME: " . $_SERVER['SERVER_NAME'] . "</p>";
?>