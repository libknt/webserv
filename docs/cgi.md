### cgi

ref
https://tex2e.github.io/rfc-translater/html/rfc3875.html
https://datatracker.ietf.org/doc/html/rfc3875

urlに基づいて実行されるCGIスクリプかどうか決定する。

GET
  GETメソッドは、スクリプトがメタ変数値に基づいてドキュメント を生成することを示す。 慣習上、GETメソッドは 安全(safe)」かつ「べき等(idempotent)」であり、ドキュメントを生成する以外の動作 ドキュメントを生成する以外のアクションを取るという意味を持ってはならない(SHOULD NOT)。

   GETメソッドの意味は、プロトコル固有のメタ変数によって修正され、改良されるかもしれない。プロトコル固有のメタ変数によって変更され、洗練されるかもしれない。
4.3.1.  GET

   The GET method indicates that the script should produce a document
   based on the meta-variable values.  By convention, the GET method is
   'safe' and 'idempotent' and SHOULD NOT have the significance of
   taking an action other than producing a document.

   The meaning of the GET method may be modified and refined by
   protocol-specific meta-variables.

POST
  POSTメソッドは、メタ変数値に加えて、リクエストメッセージボディのデータに基づいて、スクリプトが処理を実行し、ドキュメントを生成することを要求するために使用されます。 よく使われるのは
   HTML[18]におけるフォーム送信で、データベースの変更など、永続的な 影響を持つスクリプトによる処理を開始することを意図している。

   スクリプトは、添付されたmessage-bodyを読む前にCONTENT_LENGTH変数の値を チェックしなければならない[MUST]。の値をチェックしなければならない[MUST]。値をチェックすべきである(SHOULD)。
4.3.2.  POST

   The POST method is used to request the script perform processing and
   produce a document based on the data in the request message-body, in
   addition to meta-variable values.  A common use is form submission in
   HTML [18], intended to initiate processing by the script that has a
   permanent affect, such a change in a database.

   The script MUST check the value of the CONTENT_LENGTH variable before
   reading the attached message-body, and SHOULD check the CONTENT_TYPE
   value before processing it.


https://datatracker.ietf.org/doc/html/rfc3875#section-6

６．CGI 応答

6.1。応答の処理

   スクリプトは常に空ではない応答を提供しなければなりません。そのため、
   システム定義のメソッドを使用して、このデータをサーバーに送り返します。
   特に定義されていない限り、これは「標準出力」ファイルを介して行われます。
   ディスクリプタ。

   スクリプトは、処理時に REQUEST_METHOD 変数をチェックする必要があります。
   リクエストとその応答の準備。

   サーバーは、データをタイムアウト期間内に保存する必要があるタイムアウト期間を実装してもよい(MAY)。
   スクリプトから受け取りました。サーバー実装でそのようなものが定義されている場合、
   タイムアウトになり、タイムアウト期間内にスクリプトからデータを受信しません。
   サーバーはスクリプトプロセスを終了してもよい(MAY)。