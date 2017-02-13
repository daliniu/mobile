#include "util/tc_common.h"
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "util/tc_epoll_server.h"

#include <sys/un.h>
#include <iostream>

using namespace std;
using namespace taf;

/**
 * 处理类, 每个处理线程一个对象
 */
class HttpHandle : public TC_EpollServer::Handle
{
public:
    /**
     * 初始化
     */
    virtual void initialize()
    {
        cout << "HttpHandle::initialize:" << pthread_self() << endl;
    }

    /**
     * 协议解析
     * @param in
     * @param out
     * 
     * @return int
     */
    static int parse(string &in, string &out)
    {
//        LOG->debug() << in.substr(0, in.find("\r\n\r\n")) << endl;
        LOG->debug() << in << endl;

        try
        {
            bool b = TC_HttpRequest::checkRequest(in.c_str(), in.length());
            
            if(b)
            {
                out = in;
                in  = "";
                return TC_EpollServer::PACKET_FULL;
            }
            else
            {
                return TC_EpollServer::PACKET_ERR;
            }
        }
        catch(exception &ex)
        {
            return TC_EpollServer::PACKET_ERR;
        }

        return TC_EpollServer::PACKET_LESS;
    }

    /**
     * 具体处理逻辑
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
        try
        {
            TC_HttpRequest request;
            request.decode(stRecvData.buffer);

            LOG->debug() << request.getContent() << endl;

    //        cout << "**********************" << endl;
    //        cout << request.getContent() << endl;
    //        cout << "**********************" << endl;
/*
            string sFileName = "/home/jarodruan/" + TC_Common::now2str() + ".tmp";

            TC_Cgi cgi;

            //设置上传文件路径
            cgi.setUpload(sFileName, 1);//, 20);
            cgi.parseCgi(request);

            ostringstream os;
            os << TC_Common::tostr(cgi.getUploadFilesMap());

            os << TC_Common::tostr(cgi.getParamMap());
            os << cgi.isOverUploadFiles();

            TC_HttpResponse response;
            response.setResponse(200, "OK", os.str());
            response.setConnection("close");

            string s;
            for(size_t i = 0; i < 1024*1000; i++)
                 s += "a";

            response.setContent(s, true);

            string buffer = response.encode();

            sendResponse(stRecvData.uid, buffer, stRecvData.ip, stRecvData.port);
*/
//            close(stRecvData.uid);
            LOG->debug() << "handle OK" << endl;
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            LOG->debug() << ex.what() << endl;
        }
    }

    /**
     * 过载保护
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        LOG->debug() << "HttpHandle::handleTimeout:" << pthread_self() << endl;
        close(stRecvData.uid);
    }
};

