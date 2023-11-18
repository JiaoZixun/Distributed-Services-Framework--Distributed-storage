#include "service_distributed.h"


namespace sylar {


static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static std::vector<sylar::IPAddress::ptr> m_ip;
static int m_health_port;  // 健康端口
static int m_save_port;  // 存储端口
static int m_down_port;  // 下载端口
static int m_savle_nums;  // savle数量
static std::string m_fileroot = "/home/jzx/StudyCpp/Distributed_storage/Up_Files/";
static std::map<uint32_t, sylar::IPAddress::ptr> m_haship;

const uint32_t max_hash_size = UINT32_MAX - 1;

// 一致性哈希
static uint32_t getHashindex(const std::string& name) {
    std::hash<std::string> m_hash;
    return m_hash(name) % max_hash_size;
}

struct Init_Params {
    Init_Params() {
        // ip地址
        sylar::ConfigVar<std::list<std::string> >::ptr ip_list = 
            sylar::Config::Lookup("distributed_system.ip_list", std::list<std::string>{"127.0.0.1"}, "system ip_list");
        // 健康端口号
        sylar::ConfigVar<int>::ptr health_port = 
            sylar::Config::Lookup("distributed_system.health_port", (int)9090, "health port");
        // 存储端口号
        sylar::ConfigVar<int>::ptr save_port = 
            sylar::Config::Lookup("distributed_system.save_port", (int)9091, "save port");
        // 下载端口号
        sylar::ConfigVar<int>::ptr down_port = 
            sylar::Config::Lookup("distributed_system.down_port", (int)9092, "down port");
        // savle数量
        sylar::ConfigVar<int>::ptr savle_nums = 
            sylar::Config::Lookup("distributed_system.savle_nums", (int)1, "savle nums");


        YAML::Node root = YAML::LoadFile("/home/jzx/StudyCpp/Distributed_storage/distributed/service/config/config_dis.yaml");
        sylar::Config::LoadFromYaml(root);

        auto &l = ip_list->getValue();
        for(auto& x:l) {
            auto addr = sylar::IPAddress::Create(x.c_str());
            uint32_t idx = getHashindex(x);
            m_haship[idx] = addr;
            if(!addr) {
                SYLAR_LOG_INFO(g_logger) << "error ip is: " << x;   
            }
            SYLAR_LOG_INFO(g_logger) << "ip is: " << x;  
            m_ip.emplace_back(addr);
        }
        
        SYLAR_LOG_INFO(g_logger) << "health_port: " << health_port->getValue();
        m_health_port = health_port->getValue();

        SYLAR_LOG_INFO(g_logger) << "save_port: " << save_port->getValue();
        m_save_port = save_port->getValue();

        SYLAR_LOG_INFO(g_logger) << "down_port: " << down_port->getValue();
        m_down_port = down_port->getValue();

        SYLAR_LOG_INFO(g_logger) << "savle_nums: " << savle_nums->getValue();
        m_savle_nums = savle_nums->getValue();
    }
};
static struct Init_Params init_params;

Distributed::Distributed(sylar::http::HttpRequest::ptr request
                    , sylar::http::HttpResponse::ptr response
                    , sylar::http::HttpSession::ptr session) 
                : ServiceCheck(request, response, session){
    m_host = m_ip[0];   // 第一个ip为本机host
}

void Distributed::processRequest() {
    // 业务逻辑的判断
    // TODO
    HandleRequest();
}

void Distributed::Handle() {
    setCORS();  // 设置跨域
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
        const char test_response_data[] = "hello world! others";
        m_res->setBody(test_response_data);
    }
}

void Distributed::GetHandle() {
    // setCORS();  // 设置跨域
    const char test_response_data[] = "hello world! GetHandle";
    m_res->setBody(test_response_data);
    SYLAR_LOG_INFO(g_logger) << *m_res;
}

// 上传任务
void Distributed::PostHandle() {
    // setCORS();  // 设置跨域
    const char test_response_data[] = "hello world! PostHandle";
    m_res->setBody(test_response_data);
    // SYLAR_LOG_INFO(g_logger) << *m_req;
    // 上传任务：
    // 1.从Content-Type中找到分割标记
    // 2.从body中取出分割标记中间的内容
    // 3.Content-Disposition: form-data; name="file"; filename="hello.txt"，取出filename
    // 4.Content-Type: text/plain 表示文件
    // 5.剩下的是文件内容
    // int flag_pos = m_req->getHeader("Content-Type").find("boundary=") + 9;
    // std::string flag = m_req->getHeader("Content-Type").substr(flag_pos);
    // SYLAR_LOG_INFO(g_logger) << "flag: " << flag;
    // std::string body = m_req->getBody();
    // int filename_pos = body.find("filename") + 10;
    // std::string filename = "";
    // while(body[filename_pos] != '"') {
    //     filename += body[filename_pos];
    //     ++filename_pos;
    // }
    // SYLAR_LOG_INFO(g_logger) << "filename: " << filename;
    // int tmp_pos = body.find("Content-Type:");   // 类型可能有视频、文本
    // int filedata_pos_st = body.find("\n", tmp_pos)+3;     // 找到Content-Type:后面第一个空行
    // int filedata_pos_end = body.find("--"+flag+"--")-2;

    // std::string filedata = body.substr(filedata_pos_st, filedata_pos_end-filedata_pos_st);
    // // SYLAR_LOG_INFO(g_logger) << "filedata: " << filedata;
    // std::string uproot = "/home/jzx/StudyCpp/Distributed_storage/Up_Files/" + filename;
    // std::ofstream file(uproot, std::ios::binary);
    // file << filedata;
}


// 上传 对外接口
void Distributed::uploadFile() {
    setCORS();  // 设置跨域
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
    // 分片
    // 1.判断分几片，根据数据长度和机器数量
    // 2.min（机器数量，总长度/10M）
    size_t file_nums = std::max((size_t)1, std::min((size_t)m_savle_nums, filedata.size()/10240 + 1));  // 文件切片数量
    size_t file_len = filedata.size() / file_nums;  // 每片文件长度
    m_file2num[filename] = file_nums;   // 文件名：分片数量
    m_file2num[filename] = file_len;   // 文件名：分片长度
    size_t file_st = 0;
    int rt_flag = 1;
    for(size_t i = 0; i < file_nums; i++) {
        std::string file_linshi = filedata.substr(file_st, file_len);
        file_st += file_linshi.size();
        std::string frag_name = filename + "_" + std::to_string(i);
        int ok_flag = 0;
        for(int j=0;j<m_savle_nums;j++) {
            if(file2Slave(frag_name, file_linshi)) {
                ok_flag++;
            }
            if(ok_flag > m_savle_nums / 2) {    // 存储完成，直接跳出存下一个分片
                break;
            }
        }
        if(ok_flag <= m_savle_nums / 2) {    // 如果有一个分片存储没达到1/2个结点，直接返回错误
            rt_flag = 0;
            break;
        }
    }
    if(rt_flag) {
        m_res->setStatus((sylar::http::HttpStatus)200);
        m_res->setBody(filename + " upload is ok!");
    }
    else {
        m_res->setStatus((sylar::http::HttpStatus)500);
        m_res->setBody(filename + " upload is error! Please try again!");
    }
    // std::string uproot = m_fileroot + filename;
    // std::ofstream file(uproot, std::ios::binary);
    // file << filedata;
}

// 分片存储 发送给其他结点
bool Distributed::file2Slave(const std::string& frag_name, const std::string& frag_data) {
    // 1.根据分片名称找到距离最近的ip地址
    // 2.对该ip进行健康检测，访问对方的sendHealth服务
    // 3.访问对方segfileSave()服务
    // 4.接受结果 返回true
    sylar::IPAddress::ptr next_ip = getIp(frag_name);
    int error_num = 0;
    for(int i=0;i<m_savle_nums;i++) {
        if(checkHealth(next_ip)) {
            SYLAR_LOG_INFO(g_logger) <<"send to ip is: " << next_ip->toString();
            break;
        }
        ++error_num;
        if(error_num > m_savle_nums / 2) {
            SYLAR_LOG_INFO(g_logger) << "more than a half savle is error!";
            return false;
        }
        next_ip = getNextIp(next_ip);
    }
    auto res = req2service(next_ip, m_save_port, "/segfileSave", frag_name, frag_data);
    if(res->getStatus() == sylar::http::HttpStatus(200)) {
        return true;
    }
    else {
        SYLAR_LOG_INFO(g_logger) << "from " << m_host->toString() << " to " << next_ip->toString() << " port: " << m_save_port << "/segfileSave is error num: "
            << res->getStatus() << " error: " << res->getBody();
        return false;
    }
}

// 健康检测
bool Distributed::checkHealth(sylar::IPAddress::ptr ip) {
    auto res = req2service(ip, m_health_port, "/sendHealth");
    if(res->getStatus() == sylar::http::HttpStatus(200)) {
        SYLAR_LOG_INFO(g_logger) << res->getBody();
        return true;
    }
    else {
        SYLAR_LOG_INFO(g_logger) << "from " << m_host->toString() << " to " << ip->toString() << " port: " << m_save_port << "/sendHealth is error num: "
            << res->getStatus() << " error: " << res->getBody();
        return false;
    }
}

// 响应健康检测
void Distributed::sendHealth() {
    setCORS();  // 设置跨域
    m_res->setStatus((sylar::http::HttpStatus)200);
    m_res->setBody(m_host->toString() + " is ok!");
}

// 分片持久化
void Distributed::segfileSave() {
    setCORS();  // 设置跨域
    std::string file_name_seg = m_req->getHeader("segfilename", "");
    if(file_name_seg == "") {
        SYLAR_LOG_INFO(g_logger) << "segfilename is error!";
        m_res->setStatus((sylar::http::HttpStatus)500);
        m_res->setBody("segfilename is null!");
        return ;
    }
    std::string body = m_req->getBody();
    std::string uproot = m_fileroot + file_name_seg;
    std::ofstream file(uproot, std::ios::binary);
    file << body;
    m_res->setStatus((sylar::http::HttpStatus)200);
    m_res->setBody(file_name_seg);
}

// 下载文件
void Distributed::downloadFile() {
    // 1.获取待下载文件名
    // 2.判断分片数量
    // 3.组装分片名称
    // 4.从环上取地址
    // 5.请求getFileFromSlave服务，得到分片数据
    // 6.组装分片数据
    setCORS();  // 设置跨域
    std::string file_name = m_req->getHeader("filename", "");
    if(file_name == "") {
        SYLAR_LOG_INFO(g_logger) << "filename is error!";
        m_res->setStatus((sylar::http::HttpStatus)500);
        m_res->setBody("filename is null!");
        return ;
    }
    int freg_nums = m_file2num[file_name];
    // int freg_len = m_file2len[file_name];
    std::vector<std::string> all_data;
    // 根据分片数请求相应的分片内容
    for(size_t i = 0; i < freg_nums; i++) {
        std::string frag_name = file_name + "_" + std::to_string(i);
        int ok_flag = 0;
        std::string freg_data;
        if(!getFileFromSlave(frag_name, freg_data)) {  
            SYLAR_LOG_ERROR(g_logger) << "getFileFromSlave is fail!";
            m_res->setStatus((sylar::http::HttpStatus)500);
            m_res->setBody("getFileFromSlave is fail!");
            return ;
        }
        if(freg_data == "") {
            SYLAR_LOG_ERROR(g_logger) << "freg_data: "<< frag_name << "is null! ";
            m_res->setStatus((sylar::http::HttpStatus)500);
            m_res->setBody(frag_name + " is null!");
            return ;
        }
        all_data.emplace_back(freg_data);
    }
    m_res->setStatus((sylar::http::HttpStatus)200);
    m_res->setBody(merageFile(all_data));
    return ;
}


// 合并文件 返回
std::string Distributed::merageFile(const std::vector<std::string>& filedatas) {
    std::string ret;
    for(auto x:filedatas) ret+=x;
    return ret;
}

// 从其他结点获取分片内容
bool Distributed::getFileFromSlave(const std::string& frag_name, std::string& freg_data) {
    sylar::IPAddress::ptr next_ip = getIp(frag_name);
    int error_num = 0;
    for(int i=0;i<m_savle_nums;i++) {
        if(checkHealth(next_ip)) {
            SYLAR_LOG_INFO(g_logger) <<"send to ip is: " << next_ip->toString();
            break;
        }
        ++error_num;
        if(error_num > m_savle_nums / 2) {
            SYLAR_LOG_INFO(g_logger) << "more than a half savle is error!";
            return false;
        }
        next_ip = getNextIp(next_ip);
    }
    auto res = req2service(next_ip, m_down_port, "/sendSegFile", frag_name);
    if(res->getStatus() != sylar::http::HttpStatus(200)) {
        SYLAR_LOG_INFO(g_logger) << "from " << m_host->toString() << " to " << next_ip->toString() << " port: " << m_save_port << "/sendSegFile is error num: "
            << res->getStatus() << " error: " << res->getBody();
        return false;
    }
    freg_data = res->getBody();
    return true;
}

// 发送分片
void Distributed::sendSegFile() {
    setCORS();  // 设置跨域
    std::string file_name_seg = m_req->getHeader("segfilename", "");
    if(file_name_seg == "") {
        SYLAR_LOG_INFO(g_logger) << "segfilename is error!";
        m_res->setStatus((sylar::http::HttpStatus)500);
        m_res->setBody("segfilename is null!");
        return ;
    }
    std::string downroot = m_fileroot + file_name_seg;
    std::ifstream file(downroot, std::ios::binary);
    std::string ret;
    file >> ret;
    m_res->setStatus((sylar::http::HttpStatus)200);
    m_res->setBody(ret);
}


// 根据分片名得到顺时针距离最近的ip
sylar::IPAddress::ptr Distributed::getIp(const std::string& filename) {
    uint32_t idx = getHashindex(filename);
    auto it = m_haship.lower_bound(idx);
    if(it == m_haship.end()) {
        return m_haship.begin()->second;
    }
    return it->second;
}

// 根据ip得到顺时针距离最近的ip
sylar::IPAddress::ptr Distributed::getNextIp(sylar::IPAddress::ptr ip) {
    uint32_t idx = getHashindex(ip->toString());
    auto it = m_haship.upper_bound(idx);    // 越过当前ip
    if(it == m_haship.end()) {
        return m_haship.begin()->second;
    }
    return it->second;
}

// 根据ip 端口 路由 请求服务，返回响应报文
sylar::http::HttpResponse::ptr Distributed::req2service(sylar::IPAddress::ptr req_ip, int req_port, const std::string& req_route
        , const std::string& req_file_name, const std::string& req_body) {
    req_ip->setPort(req_port);
    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(req_ip);
    bool rt = sock->connect(req_ip);
    if(!rt) {
        SYLAR_LOG_INFO(g_logger) << "connect " << *req_ip << "failed";
        return ;
    }
    //SYLAR_LOG_INFO(g_logger) << "connect success";
    sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setPath(req_route);
    req->setHeader("host", m_host->toString());
    if(req_file_name != "") {
        req->setHeader("segfilename", req_file_name);   // 分片名称
    }
    if(req_body != "") {
        req->setBody(req_body);
    }
    conn->sendRequest(req);
    return conn->recvResponse();
}

}



