#!/usr/bin/env python3
import cgi
import cgitb
cgitb.enable()

# HTTPステータスコードの追加
print("HTTP/1.1 200 OK\r\n")
print("Date: Wed, 09 Nov 2023 12:00:00 GMT\r\n")
print("Server: MyServer\r\n")
print("Content-Type: text/html; charset=UTF-8\r\n")
print("Content-Length: 1054\r\n")
print("\r\n")
print("<html>\r\n")
print("<head>\r\n")
print("<title>Simple Page</title>\r\n")
print("</head>\r\n")
print("<body>\r\n")
print("<h1>Hello, World!</h1>   \r\n")
print("<p>Welcome to our website! Here you can find various resources and information.</p>\r\n")
print("<h2>About Us</h2>\r\n")
print("<p>We are a team dedicated to providing the best content and services to our community. ")
print("Our mission is to deliver informative and engaging content.</p>\r\n")
print("<h2>Features</h2>\r\n")
print("<ul>\r\n")
print("   <li>Quality Articles</li>\r\n")
print("   <li>Interactive Tutorials</li>\r\n")
print("   <li>Community Forums</li>\r\n")
print("   <li>Latest News and Updates</li>\r\n")
print("</ul>\r\n")
print("<h2>Gallery</h2>\r\n")
print("<img src=\"image1.jpg\" alt=\"Description of image 1\">\r\n")
print("<img src=\"image2.jpg\" alt=\"Description of image 2\">\r\n")
print("<img src=\"image3.jpg\" alt=\"Description of image 3\">\r\n")
print("<h2>Contact Us</h2>\r\n")
print("<p>Have questions or feedback? Reach out to us at <a ")
print("href=\"mailto:contact@example.com\">contact@example.com</a>.</p>\r\n")
print("<footer>\r\n")
print("   <p>&copy; 2023 Our Website. All rights reserved.</p>\r\n")
print("</footer>\r\n")
print("</body>\r\n")

print("</html>\r\n")
print("\r\n")
