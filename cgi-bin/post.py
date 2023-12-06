#!/usr/bin/env python

import sys

print("Content-Type: text/html")

data = sys.stdin.read()

print("")
print("<html>")
print("<head>")
print("<title>Simple Page</title>")
print("</head>")
print("<body>")
print("<h1>Hello,POST CGI!</h1>   ")
print("<p>")
print(data)
print("</p>")
print("<footer>")
print("   <p>&copy; 2023 Our Website. All rights reserved.</p>")
print("</footer>")
print("</body>")
