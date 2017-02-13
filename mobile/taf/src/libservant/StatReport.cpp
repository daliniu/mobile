#include "servant/StatReport.h"
#include "util/tc_common.h"
#include "util/tc_timeprovider.h"
#include "log/taf_logger.h"
#include <iostream>

#define MAX_REPORT_SIZE     1400
#define MIN_REPORT_SIZE     500
#define MAX_MASTER_NAME_LEN 127
#define MAX_MASTER_IP_LEN   20
#define STAT_PROTOCOL_LEN       100   //一次stat mic上报纯协议部分占用大小，用来控制udp大小防止超MTU
#define PROPERTY_PROTOCOL_LEN   50    //一次property上纯报协议部分占用大小，用来控制udp大小防止超MTU

namespace taf
{
//////////////////////////////////////////////////////////////////
//
StatReport::StatReport()
: _tTime(0)
, _iReportInterval(60000)
, _iMaxReportSize(MAX_REPORT_SIZE)
, _bTerminate(false)
, _iSampleRate(1)
, _iMaxSampleCount(500)
{
}

StatReport::~StatReport()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void StatReport::terminate()
{
    Lock lock(*this);

    _bTerminate = true;

    notifyAll();
}

void StatReport::setReportInfo(const StatFPrx& statPrx,
                       const PropertyFPrx& propertyPrx,
                       const string& strModuleName,
                       const string& strModuleIp,
                       const string& strSetDivision,
                       int iReportInterval,
                       int iSampleRate,
                       unsigned int iMaxSampleCount,
                       int iMaxReportSize)
{
    Lock lock(*this);

    _statPrx        = statPrx;

    _propertyPrx    = propertyPrx;

    //包头信息,trim&substr 防止超长导致udp包发送失败
    _strModuleName  = trimAndLimitStr(strModuleName, MAX_MASTER_NAME_LEN);

    _strIp          = trimAndLimitStr(strModuleIp, MAX_MASTER_IP_LEN);

    _tTime          = TC_TimeProvider::getInstance()->getNow();

    _iReportInterval    = iReportInterval<60000?60000:iReportInterval;

    _iSampleRate        = (iSampleRate < 1)?1: iSampleRate;

    _iMaxSampleCount    = iMaxSampleCount>500?500:iMaxSampleCount;

    if ( iMaxReportSize < MIN_REPORT_SIZE || iMaxReportSize > MAX_REPORT_SIZE )
    {
        _iMaxReportSize = MAX_REPORT_SIZE;
    }
    else
    {
        _iMaxReportSize = iMaxReportSize;
    }
    vector<string> vtSetInfo = TC_Common::sepstr<string>(strSetDivision,".");
	if (vtSetInfo.size()!=3 ||(vtSetInfo.size()==3&&(vtSetInfo[0]=="*"||vtSetInfo[1]=="*")))
	{
		_strSetArea= "";
		_strSetID  = "";
	}
    else
    {
        _strSetName         = vtSetInfo[0];
        _strSetArea         = vtSetInfo[1];
	    _strSetID           = vtSetInfo[2];
    }
    LOG->debug() << "setReportInfo Division:" << strSetDivision << " " << _strSetName << " " << _strSetArea << " " << _strSetID  << endl;
    resetStatInterv();

    if (!isAlive())
    {
        start();
    }
}

void StatReport::addStatInterv(int iInterv)
{
    Lock lock(*this);

    _vTimePoint.push_back(iInterv);

    sort(_vTimePoint.begin(),_vTimePoint.end());

    unique(_vTimePoint.begin(),_vTimePoint.end());
}

void StatReport::getIntervCount(int time,StatMicMsgBody& body)
{
    int iTimePoint = 0;
    bool bNeedInit = false;
    bool bGetIntev = false;
    if(body.intervalCount.size() == 0)  //第一次需要将所有描点值初始化为0
    {
        bNeedInit = true;
    }
    for(int i =0;i<(int)_vTimePoint.size();i++)
    {
        iTimePoint = _vTimePoint[i];
        if(bGetIntev == false && time < iTimePoint)
        {
            bGetIntev = true;
            body.intervalCount[iTimePoint]++;
            if(bNeedInit == false)
                break;
            else
                continue;
        }
        if(bNeedInit == true)
        {
           body.intervalCount[iTimePoint] = 0;
        }
    }
    return;
}

void  StatReport::resetStatInterv()
{
    _vTimePoint.clear();
    _vTimePoint.push_back(5);
    _vTimePoint.push_back(10);
    _vTimePoint.push_back(50);
    _vTimePoint.push_back(100);
    _vTimePoint.push_back(200);
    _vTimePoint.push_back(500);
    _vTimePoint.push_back(1000);
    _vTimePoint.push_back(2000);
    _vTimePoint.push_back(3000);

    sort(_vTimePoint.begin(),_vTimePoint.end());

    unique(_vTimePoint.begin(),_vTimePoint.end());
}

string StatReport::trimAndLimitStr(const string& str, uint32_t limitlen)
{
    static const string strTime = "\r\t";

    string ret = TC_Common::trim(str, strTime);

    if (ret.length() > limitlen)
    {
        ret.resize(limitlen);
    }
    return ret;
}



/*
taf.s.1 to tafs1
*/
bool StatReport::divison2SetInfo(const string& str, vector<string>& vtSetInfo)
{
    vtSetInfo = TC_Common::sepstr<string>(str,".");

	if (vtSetInfo.size() != 3 ||(vtSetInfo.size()==3&&(vtSetInfo[0]=="*"||vtSetInfo[1]=="*")))
	{
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|bad set name [" << str << endl;
		return false;
	}
	return true;
}

/*
taf.tafstat to tafstat
*/
string StatReport::getServerName(string sModuleName)
{
    string::size_type pos =  sModuleName.find(".");
    if(pos != string::npos)
    {
         return sModuleName.substr(pos + 1); //+1:过滤.
    }
    return  sModuleName;
}


void StatReport::report(const string& strModuleName,
	                  const string& setdivision,
                      const string& strInterfaceName,
                      const string& strModuleIp,
                      uint16_t iPort,
                      StatResult eResult,
                      int iSptime,
                      int iReturnValue,
                      bool bFromClient)
{
    //包头信息,trim&substr 防止超长导致udp包发送失败
    //masterIp为空服务端自己获取。

    StatMicMsgHead head;
    StatMicMsgBody body;
	string sMaterServerName = "";
	string sSlaveServerName = "";
	string appName = "";// 由setdivision生成

    if(bFromClient)
    {

		if(_strSetName.empty()) //主调(client)没有启用set
		{
			head.masterName     = _strModuleName+"@"+string(TAF_VERSION);
		}
		else
		{
			sMaterServerName = getServerName(_strModuleName);
			head.masterName = _strSetName +_strSetArea + _strSetID  + "." + sMaterServerName +"@"+string(TAF_VERSION);
		}

		if (setdivision.empty()) //被调没有启用set分组,slavename保持原样
		{
			head.slaveName	  = trimAndLimitStr(strModuleName, MAX_MASTER_NAME_LEN);
		}
		else //被调启用set分组
		{
			vector <string> vtSetInfo;
			if(divison2SetInfo(setdivision, vtSetInfo))
			{
				head.slaveSetName		 = vtSetInfo[0];
				head.slaveSetArea		 = vtSetInfo[1];
				head.slaveSetID 		 = vtSetInfo[2];

			    sSlaveServerName =	getServerName(trimAndLimitStr(strModuleName, MAX_MASTER_NAME_LEN));
				head.slaveName = vtSetInfo[0] + vtSetInfo[1]+ vtSetInfo[2]+ "." + sSlaveServerName ;
			}
			else
			{
				head.slaveName = getServerName(trimAndLimitStr(strModuleName, MAX_MASTER_NAME_LEN));
			}
		}

        head.masterIp       = "";
        head.slaveIp        = trimAndLimitStr(strModuleIp, MAX_MASTER_IP_LEN);


    }
    else
    {
        //被调上报,masterName没有set信息
        head.masterName     = trimAndLimitStr(strModuleName, MAX_MASTER_NAME_LEN);

        head.masterIp       = trimAndLimitStr(strModuleIp, MAX_MASTER_IP_LEN);


		if(_strSetName.empty()) //被调上报，slave的set信息为空
		{
			head.slaveName      = _strModuleName;//服务端version不需要上报
		}
		else
		{
			sSlaveServerName = getServerName(_strModuleName);
			head.slaveName      = _strSetName + _strSetArea + _strSetID  + "." + sSlaveServerName;
		}
		head.slaveIp             = "";

		head.slaveSetName        = _strSetName;

        head.slaveSetArea        = _strSetArea;

        head.slaveSetID          = _strSetID;
    }

    head.interfaceName  = trimAndLimitStr(strInterfaceName, MAX_MASTER_NAME_LEN);

    head.slavePort      = iPort;

    head.returnValue    = iReturnValue;

    //包体信息.
    if(eResult == STAT_SUCC)
    {
        body.count = 1;

        body.totalRspTime = body.minRspTime = body.maxRspTime = iSptime;
    }
    else if(eResult == STAT_TIMEOUT)
    {
        body.timeoutCount = 1;
    }
    else
    {
        body.execCount = 1;
    }
    submit(head, body, bFromClient);
}

void StatReport::report(const string& strMasterName,
                        const string& strMasterIp,
                        const string& strSlaveName,
                        const string& strSlaveIp,
                        uint16_t iSlavePort,
                        const string& strInterfaceName,
                        StatResult eResult,
                        int  iSptime,
                        int  iReturnValue)
{
    //包头信息,trim&substr 防止超长导致udp包发送失败
    //masterIp为空服务端自己获取。

    StatMicMsgHead head;
    StatMicMsgBody body;

    head.masterName     = trimAndLimitStr(strMasterName + "@" + string(TAF_VERSION), MAX_MASTER_NAME_LEN);
    head.masterIp       = trimAndLimitStr(strMasterIp,      MAX_MASTER_IP_LEN);
    head.slaveName      = trimAndLimitStr(strSlaveName,     MAX_MASTER_NAME_LEN);
    head.slaveIp        = trimAndLimitStr(strSlaveIp,       MAX_MASTER_IP_LEN);
    head.interfaceName  = trimAndLimitStr(strInterfaceName, MAX_MASTER_NAME_LEN);
    head.slavePort      = iSlavePort;
    head.returnValue    = iReturnValue;

    //包体信息.
    if(eResult == STAT_SUCC)
    {
        body.count = 1;

        body.totalRspTime = body.minRspTime = body.maxRspTime = iSptime;
    }
    else if(eResult == STAT_TIMEOUT)
    {
        body.timeoutCount = 1;
    }
    else
    {
        body.execCount = 1;
    }

    submit(head, body, true);
}

string StatReport::sampleUnid()
{
    char s[14]              = {0};
    time_t t                = TC_TimeProvider::getInstance()->getNow();
    int ip                  = inet_addr(_strIp.c_str());
    int thread              = syscall(SYS_gettid);
    static unsigned short n = 0;
    ++n;
    memcpy( s, &ip, 4 );
    memcpy( s + 4, &t, 4);
    memcpy( s + 8, &thread, 4);
    memcpy( s + 12, &n, 2 );
    return TC_Common::bin2str(string(s,14));
}

void StatReport::submit( StatMicMsgHead& head, StatMicMsgBody& body,bool bFromClient )
{
    Lock lock(*this);

    MapStatMicMsg& msg = (bFromClient == true)?_mStatMicMsgClient:_mStatMicMsgServer;
    MapStatMicMsg::iterator it = msg.find( head );
    if ( it != msg.end() )
    {
        StatMicMsgBody& stBody      = it->second;
        stBody.count                += body.count;
        stBody.timeoutCount         += body.timeoutCount;
        stBody.execCount            += body.execCount;
        stBody.totalRspTime         += body.totalRspTime;
        if ( stBody.maxRspTime < body.maxRspTime )
        {
            stBody.maxRspTime = body.maxRspTime;
        }
        //非0最小值
        if ( stBody.minRspTime == 0 ||(stBody.minRspTime > body.minRspTime && body.minRspTime != 0))
        {
            stBody.minRspTime = body.minRspTime;
        }
        getIntervCount(body.maxRspTime, stBody);
    }
    else
    {
        getIntervCount(body.maxRspTime, body);
        msg[head] = body;
    }
}

void StatReport::doSample(const string& strSlaveName,
                      const string& strInterfaceName,
                      const string& strSlaveIp,
                      map<string, string> &status)
{
    //////////////////////////////////////添加到Sample vector
    Lock lock(*this);

    if(_mmStatSampleMsg.size() > _iMaxSampleCount)
    {
        return;
    }

    ServantProxyThreadData* sptd = ServantProxyThreadData::getData();
    if(!sptd )
    {
        return;
    }

    string sUnId;
    StatSampleMsgHead head;
    int iDepth              = 0;
    int iWidth              = 0;
    int iParentWidth        = 0;
    head.slaveName          = strSlaveName;
    head.interfaceName      = strInterfaceName;

    //根节点根据采样比进行采样，产生唯一id
    if(sptd->data()->_sampleKey._root == true)
    {
        if(sptd->data()->_sampleKey._parentWidth == -1) //来自服务框架
        {
            sptd->data()->_sampleKey._root = false;
        }
        MMapStatSampleMsg::const_iterator it = _mmStatSampleMsg.find(head);
        if(it != _mmStatSampleMsg.end() && rand() % _iSampleRate != 0)
        {
            return;
        }
        sUnId   = sampleUnid();
    }
    else //非根节点
    {
        sUnId           = sptd->data()->_sampleKey._unid;
        iDepth          = sptd->data()->_sampleKey._depth;
        iWidth          = sptd->data()->_sampleKey._width;
        iParentWidth    = sptd->data()->_sampleKey._parentWidth;
    }
    //id为空 非采样数据
    if(sUnId.empty())
    {
        return;
    }

    StatSampleMsg  sample;
    sample.unid             = sUnId;
    sample.depth            = iDepth;
    sample.width            = iWidth;
    sample.parentWidth      = iParentWidth;
    sample.masterName       = _strModuleName;
    sample.slaveName        = strSlaveName;
    sample.interfaceName    = strInterfaceName;
    sample.masterIp         = "";
    sample.slaveIp          = strSlaveIp;

	if(LOG->IsNeedLog(TafRollLogger::INFO_LOG))
	{
	    ostringstream os;
	    sample.displaySimple(os);

	    LOGINFO("[TAF][StatReport::doSample:" << os.str()<<"]"<<endl);
	}
    _mmStatSampleMsg.insert(std::make_pair(head,sample));
    sptd->data()->_sampleKey._unid           = sUnId;
    sptd->data()->_sampleKey._depth          = iDepth;
    sptd->data()->_sampleKey._width          = iWidth+1;
    sptd->data()->_sampleKey._parentWidth    = iParentWidth;

    status[ServantProxy::STATUS_SAMPLE_KEY] = sUnId+"|"+TC_Common::tostr(iDepth)+"|"+TC_Common::tostr(iWidth);
}

int StatReport::reportMicMsg(MapStatMicMsg& msg,bool bFromClient)
{
    try
    {
       int iLen = 0;
       MapStatMicMsg  mTemp;
       MapStatMicMsg  mStatMsg;
       mStatMsg.clear();
       mTemp.clear();
       {
           Lock lock(*this);
           msg.swap(mStatMsg);
       }

	   ostringstream os;
       LOGINFO("[TAF][StatReport::reportMicMsg get size:" << mStatMsg.size()<<"]"<< endl);
       for(MapStatMicMsg::iterator it = mStatMsg.begin(); it != mStatMsg.end(); it++)
       {
           const StatMicMsgHead &head = it->first;
           int iTemLen = STAT_PROTOCOL_LEN +head.masterName.length() + head.slaveName.length() + head.interfaceName.length()
		       + head.slaveSetName.length() + head.slaveSetArea.length() + head.slaveSetID.length();
           iLen = iLen + iTemLen;
           if(iLen > _iMaxReportSize) //不能超过udp 1472
           {
               if(_statPrx)
               {
                   LOGINFO("[TAF][StatReport::reportMicMsg send size:" << mTemp.size()<<"]"<< endl);
                   _statPrx->async_reportMicMsg(NULL,mTemp,bFromClient);
               }
               iLen = iTemLen;
               mTemp.clear();
           }
           mTemp[head] = it->second;
		   if(LOG->IsNeedLog(TafRollLogger::INFO_LOG))
		   {
			   os.str("");
			   head.displaySimple(os);
			   os << "  ";
			   mTemp[head].displaySimple(os);
			   LOGINFO("[TAF][StatReport::reportMicMsg display:" << os.str() << endl);
		   }
       }
       if(0 != (int)mTemp.size())
       {
           if(_statPrx)
           {
               LOGINFO("[TAF][StatReport::reportMicMsg send size:" << mTemp.size()<<"]"<< endl);
               _statPrx->async_reportMicMsg(NULL,mTemp,bFromClient);
           }
       }
       return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << "StatReport::report catch exception:" << e.what() << endl;
    }
    catch ( ... )
    {
        LOG->error() << "StatReport::report catch unkown exception" << endl;
    }
    return -1;
}

int StatReport::reportPropMsg()
{
    try
    {
       MapStatPropMsg mStatMsg;
	   ostringstream os;
       {
           Lock lock(*this);
           for(map<string, PropertyReportPtr>::iterator it = _mStatPropMsg.begin(); it != _mStatPropMsg.end(); ++it)
           {
               StatPropMsgHead head;
               StatPropMsgBody body;
			   if (_strSetName.empty())
			   {
	               head.moduleName      = it->second->getMasterName();
	               if (head.moduleName.empty()) head.moduleName = _strModuleName;
			   }
			   else
			   {
				   string sMaterServerName = getServerName(_strModuleName);
				   head.moduleName = _strSetName +_strSetArea + _strSetID  + "." + sMaterServerName ;
			   }

               head.ip              = "";
               head.propertyName    = it->first;
               head.setName         = _strSetName;
               head.setArea         = _strSetArea;
               head.setID           = _strSetID;

               vector<pair<string, string> > v = it->second->get();
               for(size_t i = 0; i < v.size(); i++)
               {
                   StatPropInfo sp;
                   sp.policy = v[i].first;
                   sp.value  = v[i].second;
                   body.vInfo.push_back(sp);
               }
               mStatMsg[head]  = body;
			   if(LOG->IsNeedLog(TafRollLogger::INFO_LOG))
			   {
				   os.str("");
			   	   head.displaySimple(os);
			       os << "  ";
			       mStatMsg[head].displaySimple(os);
			       LOGINFO("[TAF][StatReport::reportPropMsg display:" << os.str() << endl);
			   }
           }
       }
       LOGINFO("[TAF][StatReport::reportPropMsg get size:" << mStatMsg.size()<<"]"<< endl);
       int iLen = 0;
       MapStatPropMsg mTemp;
       for(MapStatPropMsg::iterator it = mStatMsg.begin(); it != mStatMsg.end(); it++)
       {
           const StatPropMsgHead &head = it->first;
           const StatPropMsgBody &body = it->second;
           int iTemLen = head.moduleName.length()+ head.ip.length() + head.propertyName.length() + head.setName.length() + head.setArea.length() + head.setID.length();
           for(size_t i = 0; i < body.vInfo.size(); i++)
           {
               iTemLen+=body.vInfo[i].policy.length();
               iTemLen+=body.vInfo[i].value.length();
           }
           iTemLen = PROPERTY_PROTOCOL_LEN + body.vInfo.size(); //
           iLen = iLen + iTemLen;
           if(iLen > _iMaxReportSize) //不能超过udp 1472
           {
               if(_propertyPrx)
               {
                   LOGINFO("[TAF][StatReport::reportPropMsg send size:" << mTemp.size()<<"]"<< endl);
                   _propertyPrx->async_reportPropMsg(NULL,mTemp);
               }
               iLen = iTemLen;
               mTemp.clear();
           }
           mTemp[it->first] = it->second;
       }
       if(0 != (int)mTemp.size())
       {
           if(_propertyPrx)
           {
               LOGINFO("[TAF][StatReport::reportPropMsg send size:" << mTemp.size()<< "]"<< endl);
               _propertyPrx->async_reportPropMsg(NULL,mTemp);
           }
       }
       return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << "StatReport::reportPropMsg catch exception:" << e.what() << endl;
    }
    catch ( ... )
    {
        LOG->error() << "StatReport::reportPropMsg catch unkown exception" << endl;
    }
    return -1;
}

int StatReport::reportSampleMsg()
{
    try
    {
        MMapStatSampleMsg mmStatSampleMsg;
        {
            Lock lock(*this);
            _mmStatSampleMsg.swap(mmStatSampleMsg);
        }
        LOGINFO("[TAF][StatReport::reportSampleMsg get size:" << mmStatSampleMsg.size()<<"]"<< endl);
        int iLen = 0;
        vector<StatSampleMsg> vTemp;
        for(MMapStatSampleMsg::const_iterator it = mmStatSampleMsg.begin() ;it != mmStatSampleMsg.end();++it)
        {
           StatSampleMsg sample =  it->second;
           int iTemLen = STAT_PROTOCOL_LEN +sample.masterName.length() + sample.slaveName.length() + sample.interfaceName.length();
           iLen = iLen + iTemLen;
           if(iLen > _iMaxReportSize) //不能超过udp 1472
           {
               if(_statPrx)
               {
                   LOGINFO("[TAF][StatReport::reportSampleMsg send size:" << vTemp.size()<< "]"<< endl);
                   _statPrx->async_reportSampleMsg(NULL,vTemp);
               }
               iLen = iTemLen;
               vTemp.clear();
           }
           vTemp.push_back(sample);
        }
        if(0 != (int)vTemp.size())
        {
           if(_statPrx)
           {
               LOGINFO("[TAF][StatReport::reportSampleMsg send size:" << vTemp.size()<< "]"<< endl);
               _statPrx->async_reportSampleMsg(NULL,vTemp);
           }
        }

        return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << "StatReport::reportSampleMsg catch exception:" << e.what() << endl;
    }
    catch ( ... )
    {
        LOG->error() << "StatReport::reportSampleMsg catch unkown exception" << endl;
    }
    return -1;
}

void StatReport::run()
{
    while(!_bTerminate)
    {
        try
        {
            time_t tNow = TC_TimeProvider::getInstance()->getNow();

            if(tNow - _tTime > _iReportInterval/1000)
            {
                reportMicMsg(_mStatMicMsgClient, true);

                reportMicMsg(_mStatMicMsgServer, false);

                reportPropMsg();

                reportSampleMsg();

                _tTime = tNow;
            }

            Lock lock(*this);

            timedWait(1000);
        }
        catch ( exception& e )
        {
            LOG->error() << "StatReport::run catch exception:" << e.what() << endl;
        }
        catch ( ... )
        {
            LOG->error() << "StatReport::run catch unkown exception" << endl;
        }
    }
}

////////////////////////////////////////////////////////////////
}

