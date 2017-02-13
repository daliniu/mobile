#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2oc.h"


void usage()
{
    cout << "Usage : jce2oc [OPTION] jcefile" << endl;
	cout << "  --dir=DIRECTORY       generate source file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --arc    是否支持arc版本" << endl;
	cout << "  --nonamespace  设置各个成员属性是否加上名字空间前缀,默认是加上" << endl;
	cout << "  --with-namespace=NAME 自定义变量名前缀的名字空间" << endl;
    cout << "  jce2oc support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
				usage();
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

    //是否可以以taf开头
    g_parse->setTaf(option.hasParam("with-taf"));
    g_parse->setHeader(option.getValue("header"));

    Jce2OC j2c;
	if (option.hasParam("dir"))
	{
		j2c.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2c.setBaseDir(".");
	}

	if (option.hasParam("arc"))
	{
		j2c.setArc(true);
	}
	else
	{
		j2c.setArc(false);
	}

	if(option.hasParam("nonamespace"))
	{
		j2c.setNeedNS(false);

	}
	else
	{
		j2c.setNeedNS(true);
		j2c.setNS(option.getValue("with-namespace"));
	}

	try
	{
	    for(size_t i = 0; i < vJce.size(); i++)
	    {

	        g_parse->parse(vJce[i]);
	        j2c.createFile(vJce[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

