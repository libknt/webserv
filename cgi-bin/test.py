#!/usr/bin/env python

import os

print("Content-Type: text/html")  # HTMLを出力することをブラウザに伝えます
print()  # ヘッダと本文を分けるための空行

print("<html><body>")
print("<h1>Received HTTP Headers</h1>")
print("<table>")

# 環境変数からHTTPヘッダを取得し、表示します
for key, value in os.environ.items():
    if key.startswith("HTTP_"):
        print("<tr><td>{}</td><td>{}</td></tr>".format(key, value))

print("</table>")
print("</body></html>")
