#ifndef __XMLPROXY_PROTOCOL_H__
#define __XMLPROXY_PROTOCOL_H__
#include "util/tc_http.h"

struct XmlProxyProtocol
{
    static int parseXml(string &in, string &out)
    {
        try
        {
            TC_HttpRequest request;
            if(request.decode(in))
            {
                out = in;
                in  = "";
                return TC_EpollServer::PACKET_FULL;
            }
        }
        catch(exception &ex)
        {
            return TC_EpollServer::PACKET_ERR;
        }

        return TC_EpollServer::PACKET_LESS;
    }
};

#endif

