#include "util/tc_common.h"
#include "util/tc_epoll_server.h"
#include "util/tc_parsepara.h"

#include <sys/un.h>
#include <iostream>

using namespace std;
using namespace taf;

/**
 * 处理类, 每个处理线程一个对象
 */
class StringHandle : public TC_EpollServer::Handle
{
public:

    /**
     * 初始化
     */
    virtual void initialize()
    {
        cout << "StringHandle::initialize:" << pthread_self() << endl;
    }

    /**
     * 解析
     * @param in
     * @param out
     * 
     * @return int
     */
    static int parse(string &in, string &out)
    {
        string::size_type pos = in.find("\r\n");

        if(pos != string::npos)
        {
            out = in.substr(0, pos);
            in  = in.substr(pos+2);

            return TC_EpollServer::PACKET_FULL;   //返回1表示收到的包已经完全
        }

        return TC_EpollServer::PACKET_LESS;		//返回0表示收到的包不完全
//		return TC_EpollServer::PACKET_ERR;	    //返回-1表示收到包协议错误，框架会自动关闭当前连接
    }

    /**
     * 具体处理逻辑
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
        try
        {
            taf::TC_Parsepara para(stRecvData.buffer);
            cout << TC_Common::tostr(para.toMap()) << endl;
            
            sendResponse(stRecvData.uid, para.tostr(), stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

    /**
     * 过载保护
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        cout << "StringHandle::handleTimeout:" << pthread_self() << endl;
        close(stRecvData.uid);
    }
};


