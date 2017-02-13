#ifndef __ACTIVATOR_H_
#define __ACTIVATOR_H_
#include "Node.h"
#include <unistd.h>
#include "util/tc_file.h"
#include "util/tc_monitor.h"
#include <iostream>

using namespace taf;
using namespace std;
////////////////////////////////////////////////////
// 

//用来标志脚本结束
/////////////////////////////////////////////////////////
// 环境变量
struct EnvironmentEval : std::unary_function<string, string>
{
    string
    operator()(const std::string& value)
    {
        string::size_type assignment = value.find("=");
        if(assignment == string::npos || assignment >= value.size() - 1)
        {
            return value;
        }
        string v = value.substr(assignment + 1);
        assert(v.size());
        string::size_type beg = 0;
        string::size_type end;

        //宏替换
        while((beg = v.find("$", beg)) != string::npos && beg < v.size() - 1)
        {
            string variable;
            if(v[beg + 1] == '{')
            {
            end = v.find("}");
            if(end == string::npos)
            {
                break;
            }
            variable = v.substr(beg + 2, end - beg - 2);
            }
            else
            {
            end = beg + 1;
            while((isalnum(v[end]) || v[end] == '_')  && end < v.size())
            {
                ++end;
            }
            variable = v.substr(beg + 1, end - beg - 1);
            --end;
            }

            char* val = getenv(variable.c_str());
            string valstr = val ? string(val) : "";
            v.replace(beg, end - beg + 1, valstr);
            beg += valstr.size();
        }
		//此处需要马上设置，否则以上宏替换中获取的环境变量为空
		setenv((value.substr(0, assignment)).c_str(),v.c_str(),true);

        return value.substr(0, assignment) + "=" + v;
    }
};

class Activator : public TC_ThreadLock, public TC_HandleBase
{
public:
    /**
     * 构造服务
     * iTimeInterval秒内最多进行iMaxCount次启动。 达到最大启动次数仍失败后iPunishInterval秒重试启动一次
     * @param iTimeInterval
     * @param iMaxCount 
     * @param iPunishInterval 惩罚时间间隔 
     *
     */
    Activator( int iTimeInterval,int iMaxCount,int iPunishInterval)
    :_bLimited(false)
    ,_iCurCount(0)
    ,_iMaxCount(iMaxCount)
    ,_iTimeInterval(iTimeInterval)
    ,_iPunishInterval(iPunishInterval)
    ,_bTermSignal(false)
    ,_sRedirectPath("")
    {
        _vActivatingRecord.clear();
    };

    ~Activator()
    {
    };
    
    /**
     * 启动服务
     * 
     * @param strExePath 可执行文件路径
     * @param strPwdPath 当前路径 
     * @param strRollLogPath 日志路径 
     * @param vOptions 启动参数 
     * @param vEnvs 环境变量 
     * @return pid_t 生成子进程id 
     *
     */
    pid_t activate(const string& strExePath, 
                   const string& strPwdPath,
                   const string &strRollLogPath, 
                   const vector<string>& vOptions, 
                   vector<string>& vEnvs);

    /**
     * 脚本启动服务
     * 
     * @param sServerId 服务id
     * @param strStartScript 脚本路径  
     * @param strMonitorScript 脚本路径 
     * @param sResult 执行结果  
     * @return pid_t 生成子进程id 
     *
     */
    pid_t activate(const string &strServerId, 
                   const string& strStartScript,
                   const string &strMonitorScript,
                   string &strResult);

    /**
     * 停止服务
     * 
     * @param pid 进程id
     * @return int 0 成功  其它失败
     */
    int deactivate( int pid );

    /**
     * 停止服务 并生成core文件
     * 
     * @param pid 进程id
     * @return int 0 成功  其它失败
     */
    int deactivateAndGenerateCore( int pid );
    
    /**
     * 发送信号
     * 
     * @param pid   进程id 
     * @param signal 信号
     * @return int 0 成功  其它失败
     */
    int sendSignal( int pid, int signal ) const;
public:
    
    bool isActivatingLimited (); //启动限制,用来防止问题服务不断重启影响其它服务

    void addActivatingRecord();

    //运行脚本
    bool doScript(const string &sServerId,
                  const string &strScript,
                  string &strResult,
                  map<string,string> &mResult,
                  const string &sEndMark = "");

    map <string,string> parseScriptResult(const string &strServerId,const string &strResult);
	void setRedirectPath(const string& sRedirectpath) {_sRedirectPath = sRedirectpath;}

private:
    int pclose2(FILE *fp);
    FILE* popen2(const char *cmdstring, const char *type);
       
private:
    vector <time_t> _vActivatingRecord;   
    bool    _bLimited;   //是否启动受限
    int     _iCurCount;  //当前启动次数
    int     _iMaxCount;  //最大启动次数
    int     _iTimeInterval; //时间
    int     _iPunishInterval; //惩罚受限时间间隔
    
private:
    bool     _bTermSignal;      //非taf服务脚本运行超时停止信号
    string _sRedirectPath;		     //标准输出和错误输出重定向目录
};

typedef TC_AutoPtr<Activator> ActivatorPtr;
#endif

