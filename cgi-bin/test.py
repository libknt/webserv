#!/usr/bin/env python3
import cgi
import cgitb
cgitb.enable()

print("Content-Type: text/html")    # HTML is following
print()                             # blank line, end of headers

# HTMLの開始
print("<html>")
print("<head>")
print("<title>Simple CGI Script</title>")
print("</head>")
print("<body>")
print("<h1>Welcome to My CGI Script</h1>")
print("<p>This is a simple Python CGI script handling a GET request.</p>")
print("</body>")
print("</html>")
