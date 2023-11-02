#include "exec.hpp"

namespace server {

// confが出来次第、対応するconfも引数として受け取る。
HttpResponse    executeRequest(HttpRequest const &request)
{
    if (request.getHttpMethod() == "GET")
        return (executeGet(request));
    else if (request.getHttpMethod() == "POST")
        return (executePost(request));
    //else if (request.getHttpMethod() == "DELETE")
    //    return (executeDelete(request));
    return (executeError(request));
}

HttpResponse    executeGet(HttpRequest const &request)
{
    HttpResponse    response;
    struct stat file_info;
    //ここではrequest_pathにある実際のパスをconfの情報をみて見つける。なかったらエラー
    //今はconfがないので以下のダミーデータで対応。
    char    buffer[256];
    memset(buffer, '\0', 256);
    getcwd(buffer, 256);
    std::string file_path(buffer);
    file_path += "/template/index.html";
    //ここまでダミーデータのパス生成。
    response.setFilePath(file_path);
    if (stat(file_path.c_str(), &file_info) != 0)
        return (executeError(request));
    //ここからresponseのヘッダーを入れていく。今は最小限実装だが、対応したいヘッダーは順次ここで設定する。
    //TODO to_stringはC++11なはず。
    response.setHeaderValue("Content-Length", std::to_string(file_info.st_size));
    response.setHeaderValue("Content-Type", "text/html; charset=UTF-8");
    //最後にステータスコードを設定する。
    response.setStatusCode(200);
    //stream_にこれらのデータを流し込む。
    response.insertStream(request);
    return (response);
}

HttpResponse    executePost(HttpRequest const &request)
{
    HttpResponse    response;
    struct stat file_info;
    //ここではrequest_pathにある実際のパスをconfの情報をみて見つける。なかったらエラー
    //今はconfがないので以下のダミーデータで対応。
    char    buffer[256];
    memset(buffer, '\0', 256);
    getcwd(buffer, 256);
    std::string file_path(buffer);
    file_path += "/template/index.html";
    //ここまでダミーデータのパス生成。
    response.setFilePath(file_path);
    if (stat(file_path.c_str(), &file_info) != 0)
        return (executeError(request));
    //ここからresponseのヘッダーを入れていく。今は最小限実装だが、対応したいヘッダーは順次ここで設定する。
    //TODO to_stringはC++11なはず。
    response.setHeaderValue("Content-Length", std::to_string(file_info.st_size));
    response.setHeaderValue("Content-Type", "text/html; charset=UTF-8");
    //最後にステータスコードを設定する。
    response.setStatusCode(200);
    //stream_にこれらのデータを流し込む。
    response.insertStream(request);
    return (response);
}

HttpResponse    executeError(HttpRequest const &request)
{
    HttpResponse    response;
    struct stat file_info;
    //ここではrequest_pathにある実際のパスをconfの情報をみて見つける。なかったらエラー
    //今はconfがないので以下のダミーデータで対応。
    char    buffer[256];
    memset(buffer, '\0', 256);
    getcwd(buffer, 256);
    std::string file_path(buffer);
    file_path += "/template/error.html";
    //ここまでダミーデータのパス生成。
    response.setFilePath(file_path);
	//ここでエラーが生じたら一環の終わり。
    stat(file_path.c_str(), &file_info);
    //ここからresponseのヘッダーを入れていく。今は最小限実装だが、対応したいヘッダーは順次ここで設定する。
    //TODO to_stringはC++11なはず。
    response.setHeaderValue("Content-Length", std::to_string(file_info.st_size - 1));
    response.setHeaderValue("Content-Type", "text/html; charset=UTF-8");
    //最後にステータスコードを設定する。
    response.setStatusCode(400);
    //stream_にこれらのデータを流し込む。
    response.insertStream(request);
    return (response);
}

}
