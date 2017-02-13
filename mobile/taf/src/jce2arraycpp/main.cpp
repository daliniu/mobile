#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2arraycpp.h"


void usage()
{
    cout << "Usage : jce2cpp  jcefile" << endl;
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

    jce2arraycpp j2c;
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

