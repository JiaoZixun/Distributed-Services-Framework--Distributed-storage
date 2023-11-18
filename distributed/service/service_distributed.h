#ifndef __SERVICE_DISTRIBUTED_H__
#define __SERVICE_DISTRIBUTED_H__

#include "../../sylar/log.h"
#include "../../sylar/http/http.h"
#include "../../sylar/http/http_connection.h"
#include "../../sylar/http/http_session.h"
#include "../../sylar/config.h"

#include <yaml-cpp/yaml.h>
#include "service_check.h"

#include <map>
#include<cmath>
#include<algorithm>


namespace sylar {

class Distributed : public ServiceCheck {
public:
    typedef std::shared_ptr<Distributed> ptr;

    Distributed(sylar::http::HttpRequest::ptr request
                    , sylar::http::HttpResponse::ptr response
                    , sylar::http::HttpSession::ptr session);
    // Distributed();

    virtual ~Distributed() {}
    void processRequest();
    void Handle() override;
    void GetHandle();
    void PostHandle();

    // 上传 对外接口
    void uploadFile();

    // 分片存储 发送给其他结点
    bool file2Slave(const std::string& frag_name, const std::string& frag_data);

    // 分片持久化
    void segfileSave();

    // 健康检测
    bool checkHealth(sylar::IPAddress::ptr ip);

    // 响应健康检测
    void sendHealth();

    // 下载文件
    void downloadFile();

    // 合并文件 返回
    std::string merageFile(const std::vector<std::string>& filedatas);

    // 从其他结点获取分片内容
    bool getFileFromSlave(const std::string& frag_name, std::string& freg_data);

    // 发送分片
    void sendSegFile();

private:
    // 根据分片名得到顺时针距离最近的ip
    sylar::IPAddress::ptr getIp(const std::string& name);

    // 根据ip得到顺时针距离最近的ip
    sylar::IPAddress::ptr getNextIp(sylar::IPAddress::ptr ip);

    // 根据ip 端口 路由 请求服务，返回响应报文
    sylar::http::HttpResponse::ptr req2service(sylar::IPAddress::ptr req_ip, int req_port, const std::string& req_route, const std::string& req_file_name = "", const std::string& req_body = "");

private:
    sylar::IPAddress::ptr m_host;
    std::unordered_map<std::string, int> m_file2num;    // 文件名：分片数量
    std::unordered_map<std::string, int> m_file2len;    // 文件名：分片长度
};



}


#endif