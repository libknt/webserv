import cgi
import cgitb
cgitb.enable()

# HTTPステータスコードの追加
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
