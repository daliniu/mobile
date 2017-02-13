#include "util/tc_option.h"
#include "util/tc_common.h"
#include "util/tc_file.h"
#include "parse.h"
#include "hce2cpp.h"

void usage()
{
    cout << "Usage : hce2cpp [OPTION] hcefile" << endl;
    exit(0);
}

void check(vector<string> &vHce)
{
    bool b = true;

    for(size_t i  = 0; i < vHce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vHce[i]);
        if(ext == "hce")
        {
            if(!b)
            {
                usage();
            }
            if(!taf::TC_File::isFileExist(vHce[i]))
            {
                cerr << "file '" << vHce[i] << "' not exists" << endl;
                exit(0);
            }
        }
        else
        {
            cerr << "only support hce file." << endl;
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
    vector<string> vHce = option.getSingle();

	check(vHce);

    if(option.hasParam("help"))
    {
        usage();
    }

	Hce2Cpp h2c;
	try
	{
	    for(size_t i = 0; i < vHce.size(); i++)
	    {
	        g_parse->parse(vHce[i]);
	        h2c.createFile(vHce[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}


