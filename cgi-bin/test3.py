#!/usr/bin/python3
import cgi
import cgitb
cgitb.enable()

print("Status: 200 OK")
print("Content-Type: text/html; charset=UTF-8")
print("")
print("<html>")
print("<head>")
print("<title>Simple Page</title>")
print("</head>")
print("<body>")
print("<h1>Hello, CGI!</h1>   ")
print("</body>")

print("</html>")
print("")
