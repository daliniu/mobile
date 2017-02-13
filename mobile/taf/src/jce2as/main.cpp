#include "util/tc_option.h"
#include "util/tc_file.h"
#include "jce2as.h"

void usage()
{
    cout << "Usage : jce2as [OPTION] jcefile" << endl;
	cout << "     --dir=DIRECTORY  generate source file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
	cout << "  jce2c support type: bool byte short int long float double vector map" << endl;
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

	Jce2as j2as;

	if (option.hasParam("dir"))
	{
		j2as.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2as.setBaseDir(".");
	}



    j2as.setBasePackage("com.qq.");
	try
	{
	    for (size_t i = 0; i < vJce.size(); i++)
	    {
	        g_parse->parse(vJce[i]);
	        j2as.createFile(vJce[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

