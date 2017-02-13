#include "CheckSettingState.h"
#include "log/taf_logger.h"


CheckSettingState::CheckSettingState()
: _bTerminate(false)
, _iCheckingInterval(10)
, _iLeastChangedTime(10*60)
{
}

CheckSettingState::~CheckSettingState()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}


int CheckSettingState::init()
{
    LOG->debug()<<"begin CheckSettingStateThread init"<<endl;

	//初始化配置db连接
	extern TC_Config * g_pconf;
	_db.init(g_pconf);

	//轮询server状态的间隔时间
	_iCheckingInterval = TC_Common::strto<int>((*g_pconf)["/taf/reap<queryInterval>"]);

	_iCheckingInterval 	  = _iCheckingInterval 	  < 5 ? 5 : _iCheckingInterval;

	_iLeastChangedTime = TC_Common::strto<int>((*g_pconf).get("/taf/reap<querylesttime>", "600"));
	_iLeastChangedTime = _iLeastChangedTime < 60 ? 60 : _iLeastChangedTime;

    LOG->debug()<<"CheckSettingStateThread init ok."<<endl;

    return 0;
}

void CheckSettingState::terminate()
{
    LOG->debug() << "[CheckSettingStateThread terminate.]" << endl;
    _bTerminate = true;
}

void CheckSettingState::run()
{
    time_t tLastQueryServer = 0;
    time_t tNow;

    while(!_bTerminate)
    {
        try
        {
            tNow = TC_TimeProvider::getInstance()->getNow();

            //核查各server在node的设置状态
            if(tNow - tLastQueryServer >= _iCheckingInterval)
            {
                _db.checkSettingState(_iLeastChangedTime);
				//执行完成后再赋当前时间值
				tLastQueryServer = TC_TimeProvider::getInstance()->getNow();
            }

            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            LOG->error() <<"CheckSettingState exception:" << ex.what() << endl;
        }
        catch(...)
        {
            LOG->error() <<"CheckSettingState unknown exception:" << endl;
        }
    }
}

