#!/usr/bin/python

import sys
import os

def parse_post_data(data):
    parsed_data = {}
    parameters = data.split('&')
    for param in parameters:
        key_value = param.split('=')
        if len(key_value) == 2:
            key, value = key_value
            parsed_data[key] = value
    return parsed_data

print("Content-Type: text/html\n")

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length)
parsed_data = parse_post_data(post_data)

print("<html>")
print("<head>")
print("<title>Simple Page</title>")
print("</head>")
print("<body>")
print("<h1>Hello, POST CGI!</h1>")
print("<p>")
for key, value in parsed_data.items():
    print(key)
    print(": ")
    print(value)

print("</p>")
print("<footer>")
print("<p>&copy; 2023 Our Website. All rights reserved.</p>")
print("</footer>")
print("</body>")
print("</html>")
