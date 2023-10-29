#ifndef __SYLAR_SERVICE_BASE_H__
#define __SYLAR_SERVICE_BASE_H__

#include "sylar/log.h"
#include "sylar/http/http.h"
#include "sylar/http/http_session.h"

namespace sylar {

class ServiceBase {
public:
    typedef std::shared_ptr<ServiceBase> ptr;
    ServiceBase(sylar::http::HttpRequest::ptr request
                    , sylar::http::HttpResponse::ptr response
                    , sylar::http::HttpSession::ptr session);
    virtual ~ServiceBase() {}

    bool isParamOk();
    // 虚函数，实际处理逻辑
    virtual void Handle() = 0;
    virtual void GetHandle() = 0;
    virtual void PostHandle() = 0;
    
    void setCORS() {
        // 配置跨域请求
        m_res->setHeader("Access-Control-Allow-Origin", "http://localhost:8080");
        m_res->setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        m_res->setHeader("Access-Control-Allow-Headers", "Content-Type, credentials");
        m_res->setHeader("Access-Control-Allow-Credentials", "true");
    }
protected:
    sylar::http::HttpRequest::ptr m_req;
    sylar::http::HttpResponse::ptr m_res;
    sylar::http::HttpSession::ptr m_sess;

};
}


#endif