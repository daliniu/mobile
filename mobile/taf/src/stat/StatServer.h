#include "servant/Application.h"
#include "servant/StatF.h"
#include "ReapSSDThread.h"

using namespace taf;

class StatServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 每个进程都会调用一次
     */
    virtual void destroyApp();

public:

	void getTimeInfo(time_t &tTime,string &sDate,string &sFlag);

	bool cmdSetRandOrder(const string& command, const string& params, string& result);

	//获取主调虚拟ip映射
	map<string, string>& getVirtualMasterIp(void);

	//判断是否允许进行MTT域的过滤
	bool IsEnableMTTDomain(void);

	string getRandOrder(void);

	string getClonePath(void);

	int getInserInterv(void);

private:
    void initHashMap();
	ReapSSDThread* _pReapSSDThread;

	//主调虚拟ip配置
	map<string, string> _mVirtualMasterIp;

	//针对MTT 统计特殊需求的开关
	bool _bEnableMTTDomain;

	// 随机入库开关
	string  _sRandOrder;

	//数据换存目录
	string _sClonePath;

    //数据库插入间隔,单位分钟
    int _iInsertInterval;
};

extern StatServer g_app;

