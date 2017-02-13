#include <sys/un.h>
#include <iostream>
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "util/tc_http.h"

using namespace std;
using namespace taf;


int main(int argc, char *argv[])
{
    bool printDetail = false;
    cout << "argc:" << argc << endl;
    if(argc > 1)
    {
        printDetail = true;
    }

    TC_HttpRequest request;
    
    //±éÀúÄ¿Â¼
    vector<string> vFiles ;
    TC_File::listDirectory(".",vFiles,false);
    string requestUrl = "http://172.27.34.213:8084";
    cout << requestUrl << endl;

    vector<string>::const_iterator it;
    for(it = vFiles.begin(); it != vFiles.end(); it ++)
    {
        string strFileName = (*it);
        string strPrefix = "./request.";
        if(strFileName.find(strPrefix) != 0)
        {
            continue;
        }

        cout << "find request.xml:" << strFileName  << endl;
        int intPrefixLen = strPrefix.length();
        string strKey = strFileName.substr(intPrefixLen,strFileName.length() - intPrefixLen - 4 );
        cout << "strKey:" << strKey << endl;
        
        string content = TC_File::load2str(strFileName);

        //cout << "reqContent:" << content << endl;
        request.setPostRequest(requestUrl,content);
        request.setHeader("Protocal","xml");
        TC_HttpResponse res;
        request.doRequest(res);

        string strResponse = res.getContent() ;
        
        if(printDetail)
        {
            cout << "server return:" << strResponse << endl;
        }

        string strLocalResponse = TC_File::load2str("./response." + strKey + ".xml");

        if(strResponse != strLocalResponse || strResponse.length() == 0) 
        {
            cout << strKey << " test failed" << endl;
            cout << "response:" << endl;
            cout << strResponse << endl;
            TC_File::save2file("strResponse",strResponse);
            cout << "local:" << endl;
            cout << strLocalResponse << endl;
            break;
        }
        else
        {
            cout << strKey << ": test success,¡¡strResponse.length():"<<strResponse.length()<< endl;
        }
    }

    /*
    string content = TC_File::load2str("request.xml");

    cout << "reqContent:" << content << endl;
    request.setPostRequest("http://172.27.205.110:8084",content);
    TC_HttpResponse res;
    request.doRequest(res);
    cout << res.getContent() << endl;
    */

}

