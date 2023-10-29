#include "service_check.h"
#include "sylar/http/http_parser.h"


namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

ServiceCheck::ServiceCheck(sylar::http::HttpRequest::ptr request
                    , sylar::http::HttpResponse::ptr response
                    , sylar::http::HttpSession::ptr session)
        : ServiceBase(request, response, session) {

}

// 子类函数调用父类函数
void ServiceCheck::HandleRequest() {
    // 检测参数是否合格
    if(!isParamOk()) {
        SYLAR_LOG_ERROR(g_logger) << "parameter is error!";
    }
    // 解析post请求的参数
    // TODO
    // SYLAR_LOG_INFO(g_logger) << *m_req;
    // SYLAR_LOG_INFO(g_logger) << "-------------------";
    // SYLAR_LOG_INFO(g_logger) << m_req->getParam("image_url", "defalt");
    // SYLAR_LOG_INFO(g_logger) << "-------------------";
    // SYLAR_LOG_INFO(g_logger) << m_req->getParam("account", "defalt");
    // SYLAR_LOG_INFO(g_logger) << "-------------------";
    // SYLAR_LOG_INFO(g_logger) << m_req->getParam("password", "defalt");
    // SYLAR_LOG_INFO(g_logger) << "-------------------";
    // SYLAR_LOG_INFO(g_logger) << m_req->getBody();
    // GET方法
    if(m_req->getMethod() == 1) {
        GetHandle();
    }
    // POST方法
    else if(m_req->getMethod() == 3) {
        PostHandle();
    }
    // 其他方法
    else {
        SYLAR_LOG_INFO(g_logger) << "m_req->getMethod(): " << m_req->getMethod();
        // 父类为虚，实际实现交由上层
        Handle();
    }
    
}

}