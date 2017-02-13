#ifndef _WUPREQUEST_H
#define _WUPREQUEST_H

string g_sWupPacket = "module Ttaf\n"
"{\n"
"    struct RequestPacket\n"
"    {\n"
"        1  require short        iVersion;\n"
"       2  require byte         cPacketType  = 0;\n"
"        3  require int          iMessageType = 0;\n"
"        4  require int          iRequestId;\n"
"        5  require string       sServantName = \"\";\n"
"        6  require string       sFuncName    = \"\";\n"
"        7  require vector<byte> sBuffer;\n"
"        8  require int          iTimeout     = 0;\n"
"        9  require map<string, string> context;\n"
"        10 require map<string, string> status; \n"
"    };\n"

"    struct ResponsePacket\n"
"    {\n"
"        1 require short         iVersion;\n"
"        2 require byte          cPacketType  = 0;\n"
"        3 require int           iRequestId;\n"
"       4 require int           iMessageType = 0;\n"
"       5 require int           iRet         = 0;\n"
"        6 require vector<byte>  sBuffer;\n"
"        7 require map<string, string> status; \n"
"        8 optional string        sResultDesc; \n"
"    };\n"
"};\n" ;


#endif

