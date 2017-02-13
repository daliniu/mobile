#include <iostream>
#include <map>
#include "QueryFCoder.h"
#include "util/tc_clientsocket.h"

using namespace std;
using namespace taf;

int main(int argc, char *argv[])
{
    try
    {
        string id="Comm.DbServer.DbObj";
        //map<string, string> context;
        //string req = QueryFCoder::encode_findObjectById4All("taf.tafregistry.QueryObj", id, context);
        string req = QueryFCoder::encode_findObjectById4All("taf.tafregistry.QueryObj", id);


        TC_TCPClient * clt = new TC_TCPClient("172.25.38.67", 17890, 5000);
        clt->send(req.c_str(), req.length());
        cout << "send size:" << req.length() << endl;

        char buffer[20];
        size_t len=sizeof(buffer);
        vector<taf::EndpointF> eps1, eps2;
        int iRet=0, iFuncRet=0;
        string sRecv;
        do{
            int iRecvRet = clt->recv((char*)buffer, len);

            cout << "recv ret: " << iRecvRet << " size:" << len << endl;

            sRecv.append(buffer, len);
            iRet = QueryFCoder::decode_findObjectById4All(sRecv, iFuncRet, eps1, eps2);

            cout << "decode ret:" << iRet << endl;

        } while(iRet == QueryFCoder::eJcePacketLess);

        cout<<id<<" decode ret: " << iRet << " active eps : "  << endl; 
        for(size_t i=0; i<eps1.size(); i++)
        {
            cout << i << endl;
            cout << eps1[i].host << ":" << eps1[i].port << endl;
        }
        cout << "inactive eps:" << endl;
        for(size_t i=0; i<eps2.size(); i++)
        {
            cout << i << endl;
            cout << eps2[i].host << ":" << eps2[i].port << endl;
        }
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}


