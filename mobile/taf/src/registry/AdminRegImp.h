#ifndef __ADMIN_IMP_H__
#define __ADMIN_IMP_H__

#include "AdminReg.h"
#include "EndpointF.h"
#include "DbHandle.h"

using namespace taf;

/**
 *关联控制接口类
 */

class AdminRegImp: public AdminReg
{
public:
    /**
     * 构造函数
     */
    AdminRegImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};

public:

    /***********application****************/

    /**
     * 获取application列表
     *
     * @param null
     * @param out result : 结果描述
     *
     * @return application列表
     */
    virtual vector<string> getAllApplicationNames(string &result, taf::JceCurrentPtr current);


    /***********node****************/

    /**
     * 获取node列表
     *
     * @param null
     * @param out result : 结果描述
     *
     * @return node 列表
     */
    virtual vector<string> getAllNodeNames(string &result, taf::JceCurrentPtr current);

    /**
     * 获取node版本 
     * @param name   node名称
     * @param version   node版本 
     * @param out result 结果描述
     * @return  0-成功 others-失败
     */
    virtual int getNodeVesion(const string &nodeName, string &version, string & result, taf::JceCurrentPtr current);

    /**
     * ping node
     *
     * @param name: node id
     * @param out result : 结果描述
     *
     * @return : true-ping通；false-不通
     */
    virtual bool pingNode(const string & name, string &result, taf::JceCurrentPtr current);

    /**
     * 停止 node
     *
     * @param name: node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int shutdownNode(const string & name, string &result, taf::JceCurrentPtr current);

    /**
     * 获取server列表
     *
     * @param name: null
     * @param out result : 结果描述
     *
     * @return: server列表及相关信息
     */
    virtual vector<vector<string> > getAllServerIds(string &result, taf::JceCurrentPtr current);


    /**
     * 获取特定server状态
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeNmae   : node id
     * @param out state  : 状态
     * @param out result : 结果描述
     *
     * @return : 处理结果
     */

    virtual int getServerState(const string & application, const string & serverName, const string & nodeName,
            ServerStateDesc &state, string &result, taf::JceCurrentPtr current);
    
    
     /**
     * 获取特定ip所属group
     *
     * @param sting: ip
     * @param out int  : group id
     * @param out result : 结果描述
     *
     * @return : 处理结果
     */

    virtual int getGroupId(const string & ip,int &groupId, string &result, taf::JceCurrentPtr current);



    /**
     * 启动特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int startServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * 停止特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int stopServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * 重启特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int restartServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * 通知服务
     * @param application
     * @param serverName
     * @param nodeName
     * @param command
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer(const string & application, const string & serverName, const string & nodeName,
            const string &command, string &result, taf::JceCurrentPtr current);

    /**
     * 发布特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param shutdown   : 是否需要停止server
     * @param version    : 发布的版本标识
     * @param user       : 发布者
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int patchServer(const string & application, const string & serverName, const string & nodeName,
            bool shutdown, const string & version, const string & user, string &result, taf::JceCurrentPtr current);

    /**
     * 批量发布
     *
     * @param PatchRequest : 发布请求
     * @param out result   : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int batchPatch(const taf::PatchRequest & req, string &result, taf::JceCurrentPtr current);

    /**
     * 发布特定文件
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param shutdown   : 是否需要停止server
     * @param version    : 发布的版本标识
     * @param user       : 发布者
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int patchSubborn(const string & application, const string & serverName, const string & nodeName, 
                             const string & sSrcFile, const string & sDstFile, string &result, taf::JceCurrentPtr current);

    /**
    * 获取服务发布进度
    * @param application  服务所属应用名
    * @param serverName  服务名 
    * @param nodeName   :node id 
    * @out tPatchInfo  :发布百分比
    * @return :0-成功 others-失败
    */
    virtual int getPatchPercent(const string &application, const string &serverName,const string & nodeName,
            PatchInfo &tPatchInfo, taf::JceCurrentPtr current);

    /**
     * 加载特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int loadServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * 获取相应模板
     *
     * @param profileName: 模板名称
     * @param out profileTemplate: 模板内容
     * @param out resultDesc: 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int getProfileTemplate(const std::string & profileName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current);

    /**
     * 获取务服相应模板
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out profileTemplate: 模板内容
     * @param out resultDesc: 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int getServerProfileTemplate(const string & application, const string & serverName, const string & nodeName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current);

       
protected:

    //数据库操作类对象
    CDbHandle _db;
};

#endif
