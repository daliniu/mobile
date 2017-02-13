#include "util/tc_common.h"
#include "util/tc_epoll_server.h"
#include "servant/AppProtocol.h"
#include "servant/ServantHandle.h"
#include <sys/un.h>
#include <iostream>

using namespace std;
using namespace taf;

TC_RollLogger g_logger;

/**
 * 处理类, 每个处理线程一个对象
 */
class EchoHandle : public TC_EpollServer::Handle
{
public:
    /**
     * 协议解析
     * @param in
     * @param out
     * 
     * @return int
     */
    static int parse(string &in, string &out)
    {
        return AppProtocol::parse(in, out);

        if(in.at(in.length() - 1) == '\n')
        {
            out = in;
            in  = "";
            return TC_EpollServer::PACKET_FULL;
        }
        
        return TC_EpollServer::PACKET_LESS;
    }

    /**
     * 初始化
     */
    virtual void initialize()
    {
        cout << "EchoHandle::initialize:" << pthread_self() << endl;
    }
/*
    void jce(const TC_EpollServer::tagRecvData &stRecvData)
    {
        int64_t n = TC_Common::now2us();
        
        JceCurrentPtr current = new JceCurrent(NULL);//this);

        try
        {
            current->initialize(stRecvData);
        }
        catch (JceDecodeException &ex)
        {
            LOG->error() << "[TAF]ServantHandle::handle request protocol decode error:" << ex.what() << endl;

            close(stRecvData.uid);

            return;
        }

        taf::JceOutputStream<taf::BufferWriter> _os;
        _os.write(0, 0);
        _os.write("abc", 2);
        string s;
        s.assign(_os.getBuffer(), _os.getLength());

        string r = current->encodeResponseBuffer(0,s);

        int64_t n1 = TC_Common::now2us();
        LOG->debug() << n1-n << endl;

        sendResponse(stRecvData.uid, r, stRecvData.ip, stRecvData.port);
    }
*/
    /**
     * 具体处理逻辑
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
//        jce(stRecvData);

//        sendResponse(stRecvData.uid, stRecvData.buffer, stRecvData.ip, stRecvData.port);
    }

    /**
     * 过载保护
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        cout << "EchoHandle::handleTimeout:" << pthread_self() << endl;
        close(stRecvData.uid);
    }
};

