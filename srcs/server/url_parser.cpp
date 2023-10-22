#include <iostream>
#include <sstream>
#include <string>

char decode(const std::string& str) {
	unsigned int value;
	std::istringstream in(str);
	in >> std::hex >> value;
	return static_cast<char>(value);
}

std::string url_decode(const std::string& input) {
	std::ostringstream out;
	for (size_t i = 0; i < input.length(); ++i) {
		if (input[i] == '%' && i + 2 < input.length()) {
			out << decode(input.substr(i + 1, 2));
			i += 2;
		} else {
			out << input[i];
		}
	}
	return out.str();
}

size_t findquery(std::string script) {
	size_t posQuery = script.find('?');
	size_t posSlash = script.find('/');
	if (posQuery == std::string::npos && posSlash == std::string::npos) {
		return std::string::npos;
	} else if (posQuery != std::string::npos && posSlash == std::string::npos) {
		return posQuery;
	} else if (posQuery == std::string::npos && posSlash != std::string::npos) {
		return posSlash;
	}

	return posQuery;
}

int url_query_parser(const char* url, std::string& script, std::string& query) {
	std::string u(url);

	size_t pos = u.find("cgi-bin/");
	if (pos == std::string::npos) {
		std::cerr << "not found url[cgi-bin]" << std::endl;
		return -1;
	}

	u = u.substr(pos);
	size_t posQuery = findquery(u.substr(8));
	if (posQuery == std::string::npos) {
		script = u.substr(0, 8);
		query.clear();
		return 0;
	}
	script = u.substr(0, posQuery + 8);
	query = u.substr(posQuery + 9);
	query = url_decode(query);

	return 0;
}

// int main() {
// 	std::string script("");
// 	std::string query("");
// 	url_query_parser("http://example.com/cgi-bin/script.cgi?key=value", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser(
// 		"http://example.com/cgi-bin/script.cgi?key1=value1&key2=value2", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser("http://example.com/cgi-bin/script.cgi?query=Hello%20World", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser("http://example.com/cgi-bin/script.cgi?item=apple&item=banana", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser(
// 		"http://example.com/cgi-bin/script.cgi?user[name]=John&user[age]=25", script, query);
// 	script.clear();
// 	query.clear();
// 	std::cout << std::endl;
// 	url_query_parser("http://example.com/cgi-bin/script.cgi/key=value", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser(
// 		"http://example.com/cgi-bin/script.cgi/key1=value1&key2=value2", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser("http://example.com/cgi-bin/script.cgi/query=Hello%20World", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser("http://example.com/cgi-bin/script.cgi/item=apple&item=banana", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser(
// 		"http://example.com/cgi-bin/script.cgi/user[name]=John&user[age]=25", script, query);
// 	script.clear();
// 	query.clear();
// 	url_query_parser("http://example.com/cgi-bin/script.cgi", script, query);
// 	script.clear();
// 	query.clear();
// 	return 0;
// }
/*
CGI (Common Gateway Interface) を介して URL に送信されるクエリは、HTTP GET リクエストの一部として
URL に追加されることが一般的です。これらのクエリは通常、?
キャラクターの後に追加され、キーと値のペアとして = で区切られ、複数のキーと値のペアは &
で区切られます。

以下は、URL に送られる可能性のある一般的なクエリのパターンの例です：

シングルクエリ:

http://example.com/cgi-bin/script.cgi?key=value
マルチクエリ:

http://example.com/cgi-bin/script.cgi?key1=value1&key2=value2
エンコードされた値:

特定の文字（例：スペース、アンパサンド、等号など）はURLでの使用に適していないため、エンコード（パーセントエンコード）されることがあります。例えば、スペースは
%20 にエンコードされます。 http://example.com/cgi-bin/script.cgi?query=Hello%20World
配列やリストのような複数の値:

いくつかの方法で配列やリストのような複数の値を表現できます。一般的な方法は、同じキーを複数回繰り返して使用することです。
http://example.com/cgi-bin/script.cgi?item=apple&item=banana
ネストされたキー:

いくつかのフレームワークやプラットフォームでは、ネストされたキーをサポートしています。これは通常、[
と ] を使用して表現されます。 http://example.com/cgi-bin/script.cgi?user[name]=John&user[age]=25
上記は基本的なパターンの一部ですが、実際のアプリケーションや使用ケースに応じて、さまざまな他のパターンや変種が存在する可能性があります。また、セキュリティ上の理由から、クエリパラメーターに機密情報やパスワードを含めることは避けるべきです。
http://example.com/cgi-bin/script.cgi/key=value のような URL
パターンは、通常のクエリ文字列の形式とは異なりますが、確かに使用することができます。

こちらは、通常のクエリ文字列（?key=value 形式）とは異なる、URL
のパスの一部としてキーと値を表現する方法です。この形式は、RESTful な API や Web
アプリケーションで見られることがよくあります。たとえば、特定のリソースやエンティティを識別するための
ID を表現する場面で使用されることがあります。

しかし、この形式には注意点がいくつかあります：

意味合い：URL
のパスは、通常、リソースの階層構造やカテゴリを表現するために使用されるため、キーと値のペアを表現する場合には、その意味合いを明確にする必要があります。

解析の複雑さ：この形式の URL
を適切に解析するには、特定のロジックやルーティングメカニズムが必要になる場合があります。

他のパラメータとの組み合わせ：この形式のキーと値のペアと、通常のクエリ文字列を同時に使用する場合、どのように解析や処理を行うかを考慮する必要があります。

結論として、http://example.com/cgi-bin/script.cgi/key=value のような URL
パターンは技術的に使用可能ですが、その使用には注意と適切な設計が必要です。


*/