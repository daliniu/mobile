#include "ReapThread.h"

ReapThread::ReapThread():
_bTerminate(false),
_iLoadObjectsInterval1(10),
_iLeastChangedTime1(60),
_iLoadObjectsInterval2(3600),
_iLeastChangedTime2(30*60),
_iNodeTimeout(150),
_iRegistryTimeout(150),
_bRecoverProtect(true),
_iRecoverProtectRate(30),
_bHeartBeatOff(false)
{
}

ReapThread::~ReapThread()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}


int ReapThread::init()
{
    LOG->debug()<<"begin ReapThread init"<<endl;

    //初始化配置db连接
    extern TC_Config * g_pconf;
    _db.init(g_pconf);

    //加载对象列表的时间间隔
    _iLoadObjectsInterval1 = TC_Common::strto<int>((*g_pconf).get("/taf/reap<loadObjectsInterval1>", "10"));
	//第一阶段加载最近时间更新的记录,默认是60秒
	_iLeastChangedTime1    = TC_Common::strto<int>((*g_pconf).get("/taf/reap<LeastChangedTime1>", "60"));

	_iLoadObjectsInterval2 = TC_Common::strto<int>((*g_pconf).get("/taf/reap<loadObjectsInterval2>", "3600"));
	//_iLeastChangedTime2    = TC_Common::strto<int>((*g_pconf).get("/taf/reap<LeastChangedTime2>", "60"));

    //node心跳超时时间
    _iNodeTimeout = TC_Common::strto<int>((*g_pconf)["/taf/reap<nodeTimeout>"]);

    //主控心跳超时时间
    _iRegistryTimeout = TC_Common::strto<int>((*g_pconf)["/taf/reap<reigistryTimeout>"]);

    //是否启用DB恢复保护功能
    _bRecoverProtect = (*g_pconf).get("/taf/reap<recoverProtect>", "Y") == "N"?false:true;

    //启用DB恢复保护功能状态下极限值
    _iRecoverProtectRate = TC_Common::strto<int>((*g_pconf).get("/taf/reap<recoverProtectRate>", "30"));

	_bHeartBeatOff = (*g_pconf).get("/taf/reap<heartbeatoff>", "N") == "Y"?true:false;

    //最小值保护
    _iLoadObjectsInterval1 = _iLoadObjectsInterval1 < 5 ? 5 : _iLoadObjectsInterval1;

	//node的超时检测不需要那么频繁
    _iNodeTimeout         = _iNodeTimeout         < 15 ? 5 : _iNodeTimeout;


    _iRegistryTimeout       = _iRegistryTimeout       < 5 ? 5 : _iRegistryTimeout;

    _iRecoverProtectRate  = _iRecoverProtectRate  < 1 ? 30: _iRecoverProtectRate;

    //加载对象列表
    _db.initFileCache();
    _db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,60,true);
	_db.loadGroupPriority();

    LOG->debug()<<"ReapThread init ok."<<endl;

    return 0;
}

void ReapThread::terminate()
{
    LOG->debug() << "[ReapThread terminate.]" << endl;
    _bTerminate = true;
}

void ReapThread::run()
{
	//增量加载服务分两个阶段
	//第一阶段加载时间
    time_t tLastLoadObjectsStep1 = TC_TimeProvider::getInstance()->getNow();

	//全量加载时间
	time_t tLastLoadObjectsStep2 = TC_TimeProvider::getInstance()->getNow();

    time_t tLastCheckNode = 0;
    time_t tLastQueryServer = 0;
    time_t tNow;
	while(!_bTerminate)
    {
        try
        {
            tNow = TC_TimeProvider::getInstance()->getNow();

            //加载对象列表
            if(tNow - tLastLoadObjectsStep1 >= _iLoadObjectsInterval1)
            {
                tLastLoadObjectsStep1 = tNow;

				_db.updateRegistryInfo2Db(_bHeartBeatOff);

				if(tNow - tLastLoadObjectsStep2 >= _iLoadObjectsInterval2)
				{
					tLastLoadObjectsStep2 = tNow;
					//全量加载,_iLeastChangedTime2参数没有意义
                	_db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,_iLeastChangedTime2,true);
				}
				else
				{
					_db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,_iLeastChangedTime1,false);
				}

				_db.loadGroupPriority();
            }

            //轮询心跳超时的node
            if(tNow - tLastCheckNode >= _iNodeTimeout)
            {
                tLastCheckNode = tNow;
                _db.checkNodeTimeout(_iNodeTimeout);
            }

            //轮询心跳超时的主控
            if(tNow - tLastQueryServer >= _iRegistryTimeout)
            {
                tLastQueryServer = tNow;
                _db.checkRegistryTimeout(_iRegistryTimeout);
            }


            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            LOG->error() <<"ReapThread exception:" << ex.what() << endl;
        }
        catch(...)
        {
            LOG->error() <<"ReapThread unknown exception:" << endl;
        }
    }
}



