#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jceAnalyzer.h"
#include "wupRequestF.h"

void usage()
{
    cout << "Usage : ./jceAnalyzer jcefile --FileBuff=jceBuffer.txt --structName=TestInfo  [--startPos=0] [--wupPutName=mystruct]" << endl;

    cout << "  jceAnalyzer        工具名" << endl;
    cout << "  jcefile            必须指定，该文件中包含待分析的jce结构名称" << endl;

    cout << "  --structName       必须指定，指定待分析的结构名，结构名必须在JCE文件名定义" << endl;


	cout << "  --FileBuff         指定待分析的jce二进制编码文件名" << endl;
	cout << "  --startPos         指定从编码文件的第几个字节开始是结构的编码，缺省从第0字节开始" << endl;

    cout << "  --wupPutName       wup编码时的属性名称，没有该选项表示待分析的二进制数据不是wup编码" << endl;
	cout << "  --dumpWup          是否输出wup包中的sBuffer数据,此功能依赖于wupPutName项，默认是不输出"<<endl;
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
			usage();
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
    vector<string> vJceFiles = option.getSingle();

    check(vJceFiles);

    if(option.hasParam("help"))
    {
        usage();
    }

    string streamFile;

    JceAnalyzer jAnalyzer;

    if(option.hasParam("FileBuff"))
    {
        streamFile = option.getValue("FileBuff");
		jAnalyzer.setStreamFileName(streamFile);
    }
	else
	{
        usage();
	}


    if(option.hasParam("structName"))
    {
		string sStructName = option.getValue("structName");
		if(!sStructName.empty())
		{
			jAnalyzer.setStructName(sStructName);
		}
		else
		{
			cout<<"structName shouldn't be empty"<<endl;
			exit(1);
		}
    }

    if(option.hasParam("startPos"))
    {
        string pos = option.getValue("startPos");
		if(taf::TC_Common::strto<int>(pos) >= 0)
		{
			jAnalyzer.setStartPos(taf::TC_Common::strto<int>(pos));
		}
		else
		{
			cout<<"startPos should be >=0 "<<endl;
			exit(1);

		}
    }

	string wupPutName = option.getValue("wupPutName");
    if( wupPutName != "")
    {
		jAnalyzer.setStartPos(4); //wup包头有4个字节的长度

		jAnalyzer.setPutName(wupPutName);

		jAnalyzer.setWup(true);
		if(option.hasParam("dumpWup"))
		{
			jAnalyzer.setDumpWup(true);
		}
		//
		taf::TC_File::save2file("./tmpWupRequestF.jce", g_sWupPacket);
	    g_parse->parse("./tmpWupRequestF.jce");

		taf::TC_File::removeFile("./tmpWupRequestF.jce", true);

		//analyze wup jce
		jAnalyzer.analyzeFile("./tmpWupRequestF.jce", streamFile);
		jAnalyzer.setWup(false);


		if (vJceFiles.size() > 0)
		{
			//需要考虑wup包中的内容在多个jce文件的情况
			g_parse->parse(vJceFiles[0]);
			//分析wup数据包中的sBuffer数据,即structName对应的数据
			jAnalyzer.analyzeWithWupBuffer(vJceFiles[0], "");
		}
		return 0;

	}
	try
	{
		for (size_t i = 0; i < vJceFiles.size(); i++)
	    {
	        g_parse->parse(vJceFiles[i]);
	        jAnalyzer.analyzeFile(vJceFiles[i], streamFile);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

