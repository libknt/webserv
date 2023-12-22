echo "curl http://localhost:8080/cgi-bin/client.py"
curl http://localhost:8080/cgi-bin/client.py

echo "curl http://localhost:8080/cgi-bin/empty_location.py"
curl http://localhost:8080/cgi-bin/empty_location.py

echo "curl http://localhost:8080/cgi-bin/client_with_document.py"
curl http://localhost:8080/cgi-bin/client_with_document.py

echo "curl http://localhost:8080/cgi-bin/local.py"
curl http://localhost:8080/cgi-bin/local.py

echo "curl http://localhost:8080/cgi-bin/test.py"
curl http://localhost:8080/cgi-bin/test.py

echo "curl http://localhost:8080/cgi-bin/test2.py"
curl http://localhost:8080/cgi-bin/test2.py

echo "curl http://localhost:8080/cgi-bin/test3.py"
curl http://localhost:8080/cgi-bin/test3.py

echo "curl -d \"param1=value1&param2=value2\"  http://localhost:8080/cgi-bin/post.py"
curl  -d "param1=value1&param2=value2"  http://localhost:8080/cgi-bin/post.py

echo "================================================================="
curl -d @data.txt http://localhost:8080/cgi-bin/post.py
# curl  -d "param2=value1&param2=value2&param3=qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"  http://localhost:8080/cgi-bin/post.py
