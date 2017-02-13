#ifndef __NODE_IMP_H_
#define __NODE_IMP_H_
#include "Node.h"
#include <unistd.h>
#include "PlatformInfo.h"
#include "Activator.h"
#include "KeepAliveThread.h"
#include "patch/taf_patch.h"

using namespace taf;
using namespace std;

class NodeImp : public Node
{
public:
    /**
     *
     */
    NodeImp()
    {
    };

    /**
     * 销毁服务
     * @param k
     * @param v
     *
     * @return
     */
    ~NodeImp()
    {
    };

    /**
    * 初始化
    */
    virtual void initialize();

    /**
    * 退出
    */
    virtual void destroy()
    {
    };

    /**
    * 销毁指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual int destroyServer( const string& application, const string& serverName, string &result,JceCurrentPtr current );

    /**
    * patch指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @param appDistrib  在patch server 上的app目录
    * @param shutdown  是否要求服务状态为disactive
    */
    virtual int patch(const string& application, const string& serverName,  bool shutdown,const string& patchServer, string &result,JceCurrentPtr current ) ;


    /**
    * 增强的发布接口
    * pushRequest 插入发布请求到队列
    * @param req  发布请求
    * @return  int 0成功 其它失败
    */
    int patchPro(const taf::PatchRequest & req, string & result, JceCurrentPtr current);


    /**
    * patch指定文件路径
    * @param srcFile        文件或者路径在patch服务器上的目录
    * @param dstFile        文件或者路径在node服务器上存放的目录
    * @param patchServer    patch服务器
    * @return  int 0成功 其它失败
    */
    virtual int patchSubborn(const string & application, const string & serverName, const string & srcFile, const string & dstFile, const string & patchServer, string & result, JceCurrentPtr current);

    /**
    * 加载指定文件
    * @param out result  失败说明
    * @return  int 0成功  非0失败
    */
    virtual int addFile(const string &application,const string &serverName,const string &file, string &result, JceCurrentPtr current);

    /**
    * 获取node名称
    * @return  string
    */
    virtual string getName( JceCurrentPtr current ) ;

    /**
    * 获取node上负载
    * @return  LoadInfo
    */
    virtual taf::LoadInfo getLoad( JceCurrentPtr current ) ;

    /**
    * 关闭node
    * @return  int
    */
    virtual int shutdown( string &result, JceCurrentPtr current );


    /**
    * 关闭nodes上所有服务
    * @return  int
    */

    virtual int stopAllServers( string &result,JceCurrentPtr current );

    /**
    * 载入指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual int loadServer( const string& application, const string& serverName, string &result, JceCurrentPtr current );

    /**
    * 启动指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual int startServer( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * 停止指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual int stopServer( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
     * 通知服务
     * @param application
     * @param serverName
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer( const string& application, const string& serverName, const string &command, string &result, JceCurrentPtr current );

    /**
    *  获取指定服务pid进程号
    * @param application  服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    int getServerPid( const string& application, const string& serverName, string &result, JceCurrentPtr current);

    /**
    *  获取指定服务registry设置的状态
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual ServerState getSettingState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * 获取指定服务状态
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  ServerState
    */
    virtual ServerState getState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * 同步指定服务状态
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    virtual int synState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * 发布服务进度
    * @param application  服务所属应用名
    * @param serverName  服务名
    * @out tPatchInfo  下载信息
    * @return  int
    */
    virtual int getPatchPercent( const string& application, const string& serverName, PatchInfo &tPatchInfo, JceCurrentPtr current);

    virtual taf::Int32 delCache(const std::string & sFullCacheName, const std::string &sBackupPath, const std::string & sKey, std::string &result,JceCurrentPtr current);

private:

    string          _sDownLoadPath; //文件下载目录
    NodeInfo        _tNodeInfo;     //node信息
    PlatformInfo    _tPlatformInfo; //平台信息
    RegistryPrx     _pRegistryPrx;  //主控代理

};

typedef TC_AutoPtr<NodeImp> NodeImpPtr;

#endif

