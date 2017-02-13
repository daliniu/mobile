#ifndef __REGISTRY_IMP_H__
#define __REGISTRY_IMP_H__

#include "util/tc_common.h"
#include "util/tc_config.h"

#include "Registry.h"
#include "DbHandle.h"

using namespace taf;


/*
 * 提供给node调用的接口类
 */
class RegistryImp: public Registry
{
public:
    /**
     * 构造函数
     */
    RegistryImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};

    /**
     * 获取数据
     * @param k
     * @param v
     *
     * @return int
     */
    int get(int &i, taf::JceCurrentPtr current);


    /**
     * node启动的时候往registry注册一个session
     *
     * @param name node名称
     * @param ni   node详细信息
     * @param li    node机器负载信息
     *
     * @return 注册是否成功
     */
    virtual int registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li, taf::JceCurrentPtr current);


    /**
     * node上报心跳负载
     *
     * @param name node名称
     * @param li    node机器负载信息
     *
     * @return 心跳接收状态
     */
    virtual int keepAlive(const string& name, const LoadInfo & li, taf::JceCurrentPtr current);


    /**
     * 获取在该node部署的server列表
     *
     * @param name    node名称
     *
     * @return server名称列表
     */
    virtual vector<ServerDescriptor> getServers(const string & nodeName, const string & app, const string & serverName, taf::JceCurrentPtr current);

    /**
     * 更新server状态
     *
     * @param nodeName : node id
     * @param app:       应用
     * @param serverName: server 名
     * @param state :  server状态
     *
     * @return server信息列表
     */
    virtual int updateServer(const string & nodeName, const string & app, const string & serverName,
            const taf::ServerStateInfo & stateInfo, taf::JceCurrentPtr current);

    /**
     * 量批更新server状态
     *
     * @param vecStateInfo : 批量server状态
     *
     * @return server信息列表
     */
	virtual int updateServerBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, taf::JceCurrentPtr current);

    /**
     * node停止，释放node的会话
     *
     * @param name    node名称
     */
    virtual int destroy(const string & name, taf::JceCurrentPtr current);


    /**
     * 上报server的taf库版本
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     * @param version:  server基于的taf版本号
     *
     * @return 0-成功 others-失败
     */
    virtual int reportVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version, taf::JceCurrentPtr current);

    
    /**
     * 获取node的模板配置
     * @param nodeName: node名称
     * @param out profileTemplate:  对应模板内容
     *
     * @return 0-成功 others-失败
     */
    virtual taf::Int32 getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,taf::JceCurrentPtr current);


    /**
     * node通过接口获取连接上主控的node ip
     * @param sNodeIp:  node 的ip
     *
     * @return 0-成功 others-失败
     */
    virtual taf::Int32 getClientIp(std::string &sClientIp,taf::JceCurrentPtr current);

    
    /**
     * 发布任务完成后，UPDATE版本号和发布人 
     * @param PatchResult: 发布结果 
     *  
     * @return 0-成功 othres-失败 
     */
    virtual taf::Int32 updatePatchResult(const PatchResult & result, taf::JceCurrentPtr current);

protected:
    //数据库操作
    CDbHandle _db;

};


#endif
