#include "util/tc_option.h"
#include "util/tc_file.h"
#include "jce2java.h"

void usage()
{
    cout << "Usage : jce2java [OPTION] jcefile" << endl;
    cout << "  jce2java support type: bool byte short int long float double vector map" << endl;
    cout << "supported [OPTION]:" << endl;
    cout << "  --help                           help,print this(帮助)" << endl;
    cout << "  --dir=DIRECTORY                  generate java file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --base-package=NAME              package prefix, default 'com.qq.'(package前缀，未指定则默认为com.qq.)" << endl;
    cout << "  --with-servant                   also generate servant class(一并生成服务端代码，未指定则默认不生成)" << endl;
    cout << "  --with-wsp                       also generate wsp class (一并生成wsp代码，未指定则默认不生成)" << endl;
	cout << "  --with-async                     generate ///@async(生成tac异步标签)" << endl;
	cout << "  --force-array                    默认将非byte的vector转list，加上该参数后转数组" << endl;
    cout << "  --no-holder                      don't create Holder类" << endl;
    cout << "  --check-default=<true,false>     如果optional字段值为默认值不打包(默认不打包)" << endl;
    cout << "  --extends-package=NAME           指定类继承的包名" << endl;
    cout << "  --with-charset                   使用charset编码方式，未指定则默认使用GBK编码" << endl;
    cout << "  --with-JavaBeanRule              支持javabean规范，未指定则默认不支持" << endl;

    cout << endl;
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

    Jce2Java j2j;

    //是否可以以taf开头
    g_parse->setTaf(option.hasParam("with-taf"));

    //设置生成文件的根目录
    if(option.getValue("dir") != "")
    {
        j2j.setBaseDir(option.getValue("dir"));
    }
    else
    {
        j2j.setBaseDir(".");
    }

    //包名前缀
    if(option.hasParam("base-package"))
    {
        j2j.setBasePackage(option.getValue("base-package"));
    }
    else
    {
        j2j.setBasePackage("com.duowan.");
    }

    //是否生成服务端类,默认不生成
    if(option.hasParam("with-servant"))
    {
        j2j.setWithServant(true);
    }
    else
    {
        j2j.setWithServant(false);
    }

    //是否生成wsp
    if(option.hasParam("with-wsp"))
    {
        j2j.setWithWsp(true);
    }
    else
    {
        j2j.setWithWsp(false);
    }

    //是否生成getter setter
    if(option.hasParam("no-gettersetter"))
    {
        j2j.setGenSetterGetter(false);
    }


	//是否生成异步标签
	if (option.hasParam("with-async"))
	{
		j2j.setWithAysnc(true);
	}
	else
	{
		j2j.setWithAysnc(false);
	}

	//强制转换数组的选项
	if (option.hasParam("force-array"))
	{
		j2j.setForceArray(true);
	}
	else
	{
		j2j.setForceArray(false);
	}

    //设置包名
    if (option.hasParam("extends-package"))
    {
        j2j.setTafPacket(option.getValue("extends-package"));
    }

    j2j.setHolder(!option.hasParam("no-holder"));
    //j2j.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "false"?false:true);
    //modify by edwardsu
    //默认optional数据全部都编码
    j2j.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "true"?true:false);


    if (option.hasParam("with-charset"))
    {
        j2j.setCharset(option.getValue("with-charset"));
    }
	else
	{
		j2j.setCharset("GBK");
	}

	//是否支持JavaBean规范

    if(option.hasParam("with-JavaBeanRule"))
    {
        j2j.setWithJbr(true);
    }
    else
    {
        j2j.setWithJbr(false);
    }
	try
	{
	    for(size_t i = 0; i < vJce.size(); i++)
	    {
	        g_parse->parse(vJce[i]);
	        j2j.createFile(vJce[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

