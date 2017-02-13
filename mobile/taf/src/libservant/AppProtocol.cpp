#include "util/tc_epoll_server.h"
#include "servant/AppProtocol.h"
#include "jce/Jce.h"
#include <iostream>

namespace taf
{

//TAFServer的协议解析器
int AppProtocol::parseAdmin(string &in, string &out)
{
    return parse(in, out);
}

void ProxyProtocol::tafRequest(const RequestPacket& request, string& buff)
{
	JceOutputStream<BufferWriter> os;

	request.writeTo(os);

	taf::Int32 iHeaderLen = htonl(sizeof(taf::Int32) + os.getLength());

	buff.clear();

    buff.reserve(sizeof(taf::Int32) + os.getLength());

	buff.append((const char*)&iHeaderLen, sizeof(taf::Int32));

	buff.append(os.getBuffer(), os.getLength());
}

////////////////////////////////////////////////////////////////////////////////////
}

