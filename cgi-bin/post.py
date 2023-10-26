#!/usr/bin/env python3

import cgi
import json
import sys

def main():
    # ヘッダ情報の設定
    sys.stdout.write("Content-type: application/json\r\n")
    sys.stdout.write("\r\n")

    # POSTデータの取得
    form = cgi.FieldStorage()
    post_data = form.value
    if post_data:
        try:
            # JSONデータのパース
            json_data = json.loads(post_data)
            username = json_data.get('username', '')
            password = json_data.get('password', '')

            # レスポンスの生成
            response = {
                'status': 'success',
                'message': f'Hello {username}, your password is {password}',
            }
        except json.JSONDecodeError:
            response = {
                'status': 'error',
                'message': 'Invalid JSON format',
            }
    else:
        response = {
            'status': 'error',
            'message': 'No data received',
        }
    
    sys.stdout.write(json.dumps(response))
    sys.stdout.write("\r\n")

if __name__ == "__main__":
    main()
