#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2cpp.h"
#include "jce2hpp.h"


void usage()
{
    cout << "Usage : jce2cpp [OPTION] jcefile" << endl;
    cout << "  --coder=Demo::interface1;Demo::interface2   create interface encode and decode api" << endl;
    cout << "  --dir=DIRECTORY                             generate source file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --pdu                                       special format for pdu-protocol" << endl;
    cout << "  --check-default=<true,false>                如果optional字段值为默认值不打包(默认打包)" << endl;
    cout << "  --lua                                       gracefully export structures to lua with luavatar" << endl;
    cout << "  --os                                        只生成jce文件中结构体编解码的程序段" << endl;
    cout << "  --json                                      生成json编解码" << endl;
    cout << "  --tafMSF                                    生成tafMSF名字空间的代码" << endl;
    cout << "  --unknown                                   生成处理jce数据流中的unkown field的代码" << endl;
    cout << "  --with-hpp                                  生成hpp文件"<<endl;
    cout << "  --no-lambda                                 异步是否支持lambda调用，默认支持" << endl;
    cout << "  jce2cpp support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    bool b = true;

    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!b)
            {
                usage();
            }
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
                exit(0);
            }
        }
        else
        {
            cerr << "only support jce file." << endl;
            exit(0);
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    taf::TC_Option option;
    option.decode(argc, argv);
    vector<string> vJce = option.getSingle();

    check(vJce);

    if(option.hasParam("help"))
    {
        usage();
    }

    bool bCoder = option.hasParam("coder");
    vector<string> vCoder;
    if(bCoder)
    {
        vCoder = taf::TC_Common::sepstr<string>(option.getValue("coder"), ";", false);
        if(vCoder.size() == 0)
        {
            usage();
            return 0;
        }
    }

    //是否可以以taf开头
    g_parse->setTaf(option.hasParam("with-taf"));
    g_parse->setHeader(option.getValue("header"));

    if(option.hasParam("with-hpp"))
    {
        Jce2Hpp j2c;

        if (option.hasParam("dir"))
        {
            j2c.setBaseDir(option.getValue("dir"));
        }
        else
        {
            j2c.setBaseDir(".");
        }

        j2c.setPDU(option.hasParam("pdu"));
        j2c.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "true"?true:false);

        //added by forrestliu @20101027 for lua support
        j2c.setLuaSupport(option.hasParam("lua"));

        //added by zhangcunli @20111205 for json support
        j2c.setJsonSupport(option.hasParam("json"));

        bool bNotUseLambda = option.hasParam("no-lambda");
        if(!bNotUseLambda)
        {
            j2c.setLambdaSupport(true);
        }

        j2c.setOnlyStruct(option.hasParam("os"));

        //added by johnson
        if (option.hasParam("tafMSF"))
        {
            j2c.setTafMSF("tafMSF");
        }
        else
        {
            j2c.setTafMSF("taf");
        }
        try
        {
            j2c.setUnknownField(option.hasParam("unknown"));
            for(size_t i = 0; i < vJce.size(); i++)
            {

                g_parse->parse(vJce[i]);
                j2c.createFile(vJce[i], vCoder);
            }
        }catch(exception& e)
        {
            cerr<<e.what()<<endl;
        }
    }
    else
    {
        Jce2Cpp j2c;

        if (option.hasParam("dir"))
        {
            j2c.setBaseDir(option.getValue("dir"));
        }
        else
        {
            j2c.setBaseDir(".");
        }

        j2c.setPDU(option.hasParam("pdu"));
        j2c.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "true"?true:false);

        //added by forrestliu @20101027 for lua support
        j2c.setLuaSupport(option.hasParam("lua"));

        //added by zhangcunli @20111205 for json support
        j2c.setJsonSupport(option.hasParam("json"));

        bool bNotUseLambda = option.hasParam("no-lambda");
        if(!bNotUseLambda)
        {
            j2c.setLambdaSupport(true);
        }
        else
        {
            j2c.setLambdaSupport(false);
        }


        j2c.setOnlyStruct(option.hasParam("os"));

        //added by johnson
        if (option.hasParam("tafMSF"))
        {
            j2c.setTafMSF("tafMSF");
        }
        else
        {
            j2c.setTafMSF("taf");
        }
        try
        {
            j2c.setUnknownField(option.hasParam("unknown"));
            for(size_t i = 0; i < vJce.size(); i++)
            {

                g_parse->parse(vJce[i]);
                j2c.createFile(vJce[i], vCoder);
            }
        }catch(exception& e)
        {
            cerr<<e.what()<<endl;
        }
    }

    return 0;
}

