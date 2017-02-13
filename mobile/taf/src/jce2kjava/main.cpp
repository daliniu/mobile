#include <string>
#include <vector>
#include <iostream>

#include "util/tc_option.h"
#include "util/tc_file.h"

#include "jce2kjavap.h"
#include "jce2kjava.h"

void print_usage(int argc, char ** argv)
{
    std::cout << "Usage : " << argv[0] << " [OPTION] jcefile" << std::endl;
    std::cout << "  " << argv[0] << " support type: bool byte short int long float double vector map" << std::endl << std::endl;
    std::cout << "supported [OPTION]:" << std::endl;
    std::cout << "  --help                            help,print this(帮助)" << endl;
    std::cout << "  --class-format                    with many class(生成1.5.4之前格式的代码，未指定生成新格式代码)" << std::endl;
    std::cout << "  --dir=DIRECTORY                   generate java file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    std::cout << "  --base-package=NAME               package prefix, default 'com.qq.'(package前缀，未指定则默认为com.qq.)" << endl;
    std::cout << "  --with-servant                    also generate servant class(一并生成服务端代码，未指定则默认不生成)" << endl;
    std::cout << "  --check-default<true,false>       类模式下，如果optional字段值为默认值不打包(默认打包)" << std::endl;
    std::cout << "  --no-equals                       类模式下，不生成比较函数，默认生成" << std::endl;
    std::cout << "  --byname                          根据结构体名称，选择生成readFrom或者writeTo" << std::endl;
    std::cout << "  --extends-package=NAME            指定类继承的包名" << endl;
    exit(0);
}

void check_jcefiles(std::vector<std::string> & vec_jcefiles)
{
    for (std::vector<std::string>::size_type i  = 0; i < vec_jcefiles.size(); i++)
    {
        if (taf::TC_File::extractFileExt(vec_jcefiles[i]) == "jce")
        {
            if(!taf::TC_File::isFileExist(vec_jcefiles[i]))
            {
				std::cerr << "file '" << vec_jcefiles[i] << "' does't exists" << std::endl;
                exit(0);
            }
        }
        else
        {
			std::cerr << "only support jce file." << std::endl;
			exit(0);
		}
	}
}

int create_clike_format(taf::TC_Option & tc_option, const std::vector<std::string> & vec_jcefiles)
{
	Jce2KJavaP j2jp;

    //是否可以以taf开头
    g_parse->setTaf(tc_option.hasParam("with-taf"));

    //设置生成文件的根目录
    if(tc_option.getValue("dir") != "")
    {
        j2jp.setBaseDir(tc_option.getValue("dir"));
    }
    else
    {
        j2jp.setBaseDir(".");
    }

    //包名前缀
    if(tc_option.hasParam("base-package"))
    {
        j2jp.setBasePackage(tc_option.getValue("base-package"));
    }
    else
    {
        j2jp.setBasePackage("com.qq.");
    }

    //是否生成服务端类,默认不生成
    if(tc_option.hasParam("with-servant"))
    {
        j2jp.setWithServant(true);
    }
    else
    {
        j2jp.setWithServant(false);
    }

    //设置包名
    if (tc_option.hasParam("extends-package"))
    {
        j2jp.setTafPacket(tc_option.getValue("extends-package"));
    }

	//开始生成文件
    for (std::vector<std::string>::size_type i = 0; i < vec_jcefiles.size(); i++)
    {
        g_parse->parse(vec_jcefiles[i]);
        j2jp.createFile(vec_jcefiles[i]);
    }

	return 0;
}

int create_class_format(taf::TC_Option & tc_option, const std::vector<std::string> & vec_jcefiles)
{
	Jce2KJava j2j;

    //是否可以以taf开头
    g_parse->setTaf(tc_option.hasParam("with-taf"));

    //设置生成文件的根目录
    if(tc_option.getValue("dir") != "")
    {
        j2j.setBaseDir(tc_option.getValue("dir"));
    }
    else
    {
        j2j.setBaseDir(".");
    }

    //包名前缀
    if(tc_option.hasParam("base-package"))
    {
        j2j.setBasePackage(tc_option.getValue("base-package"));
    }
    else
    {
        j2j.setBasePackage("com.qq.");
    }

    //是否生成服务端类,默认不生成
    if(tc_option.hasParam("with-servant"))
    {
        j2j.setWithServant(true);
    }
    else
    {
        j2j.setWithServant(false);
    }

    //设置是否检查默认值
    j2j.setCheckDefault(taf::TC_Common::lower(tc_option.getValue("check-default")) == "true"?true:false);

    //设置是否生成比较函数
    j2j.setEquals(!tc_option.hasParam("no-equals"));

    //根据结构体名称生成readFrom或则writeTo函数
    j2j.setByname(tc_option.hasParam("byname"));

    //设置包名
    if (tc_option.hasParam("extends-package"))
    {
        j2j.setTafPacket(tc_option.getValue("extends-package"));
    }

	//开始生成文件
    for (std::vector<std::string>::size_type i = 0; i < vec_jcefiles.size(); i++)
    {
        g_parse->parse(vec_jcefiles[i]);
        j2j.createFile(vec_jcefiles[i]);
    }

    return 0;
}

int main(int argc, char ** argv)
{
	taf::TC_Option tc_option;
	tc_option.decode(argc, argv);

	if (tc_option.hasParam("help") || argc < 2)
	{
		print_usage(argc, argv);
	}

	std::vector<std::string> vec_jcefiles = tc_option.getSingle();
	check_jcefiles(vec_jcefiles);
	try
	{
		return tc_option.hasParam("class-format")?create_class_format(tc_option, vec_jcefiles) : create_clike_format(tc_option, vec_jcefiles);
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}
	return -1;
}


