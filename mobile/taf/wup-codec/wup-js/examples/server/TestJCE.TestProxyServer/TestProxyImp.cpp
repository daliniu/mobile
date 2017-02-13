#include "TestProxyImp.h"
#include "TestProxyServer.h"
#include "servant/Application.h"
#include "util/tc_encoder.h"

using namespace std;
using namespace taf;

//////////////////////////////////////////////////////
void TestProxyImp::initialize()
{
    //initialize servant here:

}

//////////////////////////////////////////////////////
void TestProxyImp::destroy()
{
    //destroy servant here:
    //...
}

/**
* @Description:入口
*
* @Parameter: taf::JceCurrentPtr current
* @Parameter: vector<char> & response
* @Returns:   int -1:参数错误，-2内容或加密验证不通过，-3未登陆, -4接口异常 ,-5 应用服务返回错误, -6 RUL跳转
*/
int TestProxyImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
    LOG->debug()<<"[TestProxyImp::doRequest] incoming -----------------------------------"<<endl;
	current->setResponse(false);

    try
    {
        //HTTP请求解码
        const vector<char>& request = current->getRequestBuffer();
        taf::TC_HttpRequest httpRequest;
        httpRequest.decode(&request[0], request.size());

		string sBuff = httpRequest.getContent();
		string sFunc = "";

		//解码
		JceInputStream<> is;
		is.setBuffer(sBuff.data(), sBuff.size());
		is.read(sFunc, 0, true);
		LOG->debug() << "FuncName:" << sFunc << endl;
		//编码
		taf::JceOutputStream<taf::BufferWriter> os;

		if (sFunc == "getUserInfo")
		{
			string sName = "";
			is.read(sName, 1, true);
			LOG->debug() << "getUserInfo::" << sName << "|" << TC_Encoder::utf82gbk(sName) << endl;

			TestJCE::UserInfo user;
			int ret = getUserInfo(sName, user);


			os.write(ret, 0);
			os.write(user, 1);

			LOG->debug() << endl << TC_Common::bin2str(os.getBuffer(), os.getLength(), " ", 16) << endl;

			TestProxyImp::doSendResponse(current, os.getBuffer(), os.getLength());
			return 0;
		}
		else
		{
			os.write(-63000, 0);	//-63000表示出现了不支持的函数命令字
		}
    }
    catch(exception &ex)
    {
        LOG->error() << "[TestProxyImp::doRequest] error:" << ex.what() << endl;
    }
    catch(...)
    {
        LOG->error() << "[TestProxyImp::doRequest]exception." << endl;
    }

    return 0 ;
}

int  TestProxyImp::getUserInfo(const string &sName, TestJCE::UserInfo & stUser)
{
	stUser.stBasic.sUserID 		= "88885138";
	stUser.stBasic.sUserName	= TC_Encoder::gbk2utf8(sName);
	stUser.stBasic.sex			= TestJCE::EN_MALE;
	stUser.stBasic.iAge			= 18;
	stUser.stBasic.sAddress		= TC_Encoder::gbk2utf8(string("武汉市洪山区珞瑜路727号光谷银座4栋17层"));
	stUser.sToken				= "XXYYTSKDINNHSY;;KKSDTSS";

	return 200;
}

void TestProxyImp::doSendResponse(taf::JceCurrentPtr current, const char * szBuff, size_t sizeLen, int code, const string& about)
{
    LOG->debug() << "[TestProxyImp::doSendResponse] Before Send Http Response: ====> HttpCode:" << code << endl;

    taf::TC_HttpResponse response ;
    response.setResponse(code, about, "");
	response.setHeader("Access-Control-Allow-Origin", "*");
    response.setContentType("application/multipart-formdata");

    response.setConnection("close");
    response.setContent(string(szBuff, sizeLen));
    response.setContentLength(sizeLen);

    string buffer = response.encode();
    current->sendResponse(buffer.c_str(), buffer.length());
}
