#!/usr/bin/python

print("Content-type: text/html")
print("Location: http://127.0.0.1:8080/README.md")
print("")

print("<html>")
print("<head>")
print("<title>Redirecting...</title>")
print("</head>")
print("<body>")
print("<h1>You are being redirected</h1>")
print("<p>If you are not automatically redirected, ")
print("<a href='http://127.0.0.1:8080/README.md'>click here</a>.</p>")
print("</body>")
print("</html>")
