#ifndef _TC_IMP_H_
#define _TC_IMP_H_

#include "Hello.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

class HelloImp : public Hello
{
public:
    /**
     *
     */
    HelloImp(){};

     /**
     * 初始化，Hello的虚拟函数，HelloImp初始化时调用
     *
     * @return int
     */
    virtual void initialize()
	{
	}

    /**
     * 退出，Hello的虚拟函数，服务退出时调用
     */
	virtual void destroy(){};

    /**
     * testHello，实现jce文件中的接口
     *
     * @return int
     */
	int testHello(const string &s, string &r, JceCurrentPtr current)
	{
        LOG->debug() << s << endl;
        LOG->debug() << TC_Common::tostr(current->getContext())<< endl;
//        FDLOG("test") << s << endl;
		r = s;
	    return 0;
	}
};


#endif



