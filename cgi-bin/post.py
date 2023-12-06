#!/usr/bin/env python

import sys
import os
import urllib.parse

print("Content-Type: text/html\n")

content_length = int(os.environ.get('CONTENT_LENGTH', 0))

post_data = sys.stdin.read(content_length)

parsed_data = urllib.parse.parse_qs(post_data)

print("<html>")
print("<head>")
print("<title>Simple Page</title>")
print("</head>")
print("<body>")
print("<h1>Hello, POST CGI!</h1>")
print("<p>")
for key, values in parsed_data.items():
    for value in values:
        print(f"{key}: {value}<br>")
print("</p>")
print("<footer>")
print("<p>&copy; 2023 Our Website  << recv_result. All rights reserved.</p>")
print("</footer>")
print("</body>")
print("</html>")
print()
# sys.stdout.flush()
# sys.exit(0)

# print("<p>&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.&copy; 2023 Our Website. All rights reserved.</p>")
#  curl  -d "param1=value1&param2=value2"  http://localhost:8080/cgi-bin/post.py