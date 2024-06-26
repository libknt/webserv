#!/usr/bin/python
import cgi
import cgitb
cgitb.enable()

print("Status: 200 OK")
print("Date: Wed, 09 Nov 2023 12:00:00 GMT")
print("Server: MyServer")
print("Content-Type: text/html; charset=UTF-8")
print("Content-Length: 1054")
print("")
print("<html>")
print("<head>")
print("<title>Simple Page</title>")
print("</head>")
print("<body>")
print("<h1>Hello, CGI!</h1>   ")
print("<p>Welcome to our website! Here you can find various resources and information.</p>")
print("<h2>About Us</h2>")
print("<p>We are a team dedicated to providing the best content and services to our community. ")
print("Our mission is to deliver informative and engaging content.</p>")
print("<h2>Features</h2>")
print("<ul>")
print("   <li>Quality Articles</li>")
print("   <li>Interactive Tutorials</li>")
print("   <li>Community Forums</li>")
print("   <li>Latest News and Updates</li>")
print("</ul>")
print("<h2>Gallery</h2>")
print("<img src=\"image1.jpg\" alt=\"Description of image 1\">")
print("<img src=\"image2.jpg\" alt=\"Description of image 2\">")
print("<img src=\"image3.jpg\" alt=\"Description of image 3\">")
print("<h2>Contact Us</h2>")
print("<p>Have questions or feedback? Reach out to us at <a ")
print("href=\"mailto:contact@example.com\">contact@example.com</a>.</p>")
print("<footer>")
print("   <p>&copy; 2023 Our Website. All rights reserved.</p>")
print("</footer>")
print("</body>")

print("</html>")
print("")
