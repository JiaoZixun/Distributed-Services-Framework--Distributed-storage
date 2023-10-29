#include "service_demo.h"

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

ServiceDemo::ServiceDemo(sylar::http::HttpRequest::ptr request
                    , sylar::http::HttpResponse::ptr response
                    , sylar::http::HttpSession::ptr session)
        : ServiceCheck(request, response, session){

}

void ServiceDemo::processRequest() {
    // 业务逻辑的判断
    // TODO
    HandleRequest();
}

void ServiceDemo::Handle() {
    // SYLAR_LOG_INFO(g_logger) << m_image_url;
    // const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
    //     "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
    //     "Server: Apache\r\n"
    //     "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
    //     "ETag: \"51-47cf7e6ee8400\"\r\n"
    //     "Accept-Ranges: bytes\r\n"
    //     "Content-Length: 81\r\n"
    //     "Cache-Control: max-age=86400\r\n"
    //     "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
    //     "Connection: Close\r\n"
    //     "Content-Type: text/html\r\n\r\n"
    //     "<html>\r\n"
    //     "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
    //     "</html>\r\n";
    setCORS();  // 设置跨域
    const char test_response_data[] = "hello world!";
    m_res->setBody(test_response_data);
    // SYLAR_LOG_INFO(g_logger) << *m_res;
}

void ServiceDemo::GetHandle() {
    setCORS();  // 设置跨域
    const char test_response_data[] = "hello world! GetHandle";
    m_res->setBody(test_response_data);
    SYLAR_LOG_INFO(g_logger) << *m_res;
}

// 上传任务
void ServiceDemo::PostHandle() {
    setCORS();  // 设置跨域
    const char test_response_data[] = "hello world! PostHandle";
    m_res->setBody(test_response_data);
    // SYLAR_LOG_INFO(g_logger) << *m_req;
    // 上传任务：
    // 1.从Content-Type中找到分割标记
    // 2.从body中取出分割标记中间的内容
    // 3.Content-Disposition: form-data; name="file"; filename="hello.txt"，取出filename
    // 4.Content-Type: text/plain 表示文件
    // 5.剩下的是文件内容
    int flag_pos = m_req->getHeader("Content-Type").find("boundary=") + 9;
    std::string flag = m_req->getHeader("Content-Type").substr(flag_pos);
    SYLAR_LOG_INFO(g_logger) << "flag: " << flag;
    std::string body = m_req->getBody();
    int filename_pos = body.find("filename") + 10;
    std::string filename = "";
    while(body[filename_pos] != '"') {
        filename += body[filename_pos];
        ++filename_pos;
    }
    SYLAR_LOG_INFO(g_logger) << "filename: " << filename;
    int tmp_pos = body.find("Content-Type:");   // 类型可能有视频、文本
    int filedata_pos_st = body.find("\n", tmp_pos)+3;     // 找到Content-Type:后面第一个空行
    int filedata_pos_end = body.find("--"+flag+"--")-2;

    std::string filedata = body.substr(filedata_pos_st, filedata_pos_end-filedata_pos_st);
    // SYLAR_LOG_INFO(g_logger) << "filedata: " << filedata;
    std::string uproot = "/home/jzx/StudyCpp/sylar/Up_Files/" + filename;
    std::ofstream file(uproot, std::ios::binary);
    file << filedata;
}

}
