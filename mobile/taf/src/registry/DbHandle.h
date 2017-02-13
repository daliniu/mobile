#ifndef __DB_HANDLE_H__
#define __DB_HANDLE_H__

#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_monitor.h"
#include "util/tc_mysql.h"
#include "util/tc_file.h"
#include "jmem/jmem_hashmap.h"

#include "Registry.h"
#include "Node.h"
#include "log/taf_logger.h"

using namespace taf;

typedef map<string, ObjectItem> ObjectsCache;
typedef JceHashMap<ObjectName, ObjectItem, ThreadLockPolicy, FileStorePolicy> FileHashMap;

/**
 *  数据库操作类
 */
class CDbHandle
{
private:
	struct GroupPriorityEntry
	{
		std::string		sGroupID;
		std::string		sStation;
		std::set<int> 	setGroupID;
	};

	enum GroupUseSelect
	{
		ENUM_USE_WORK_GROUPID,
		ENUM_USE_REAL_GROUPID
	};

	//set中服务的信息
	struct SetServerInfo
	{
		bool	   bActive;
		string 	   sServant;
		EndpointF  epf;
	};

	typedef map<string,map<string, map<string,vector<CDbHandle::SetServerInfo> > > > SetDivisionCache;

public:
    /**
     * 构造函数
     */
    CDbHandle(){_bEnMultiSql = false;}

    /**
     * 初始化
     * @param pconf 配置文件
     * @return 0-成功 others-失败
     */
    int init(TC_Config *pconf);

    /**
     * 获取特定node id的对象代理
     * @param nodeName : node id
     * @return :  对象代理的智能指针
     */
    NodePrx getNodePrx(const string & nodeName);

    /**
     * 保存node注册的session
     * @param name node名称
     * @param ni   node详细信息
     * @param li    node机器负载信息
     * @return 注册是否成功
     */
    int registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li);

    /**
     * 更新node心跳时间及机器负载
     *
     * @param name    node名称
     * @param load    node机器负载信息
     * @return 0-成功 others-失败
     */
    int keepAlive(const string & name, const LoadInfo & li);

    /**
     * 获取活动node列表endpoint信息
     * @param out result 结果描述
     * @return map : 对应id node的obj
     */
    map<string, string> getActiveNodeList(string & result);

    /**
     * 获取node版本
    * @param name   node名称
     * @param version   node版本
     * @param out result 结果描述
     * @return  0-成功 others-失败
     */
    int getNodeVersion(const string &nodeName, string &version, string & result);

    /**
     * 获取在该node部署的server列表
     *
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     *
     * @return server信息列表
     */
    vector<ServerDescriptor> getServers(const string & app, const string & serverName, const string & nodeName,bool withDnsServer = false);

    /**
     * 获取server的配置模板
     * @param sTemplateName 模板名称
     * @param sResultDesc 结果描述
     * @return 模板内容
     */
    string getProfileTemplate(const string & sTemplateName, string & sResultDesc);


protected:
    /**
     * 获取server的配置模板
     * @param sTemplateName 模板名称
     * @param mapRecursion 被递归到的模板
     * @param sResultDesc 结果描述
     * @return 模板内容
     */
    string getProfileTemplate(const string & sTemplateName, map<string, int> & mapRecursion, string & sResultDesc);

public:
    /**
     * 更新server状态
     *
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     * @param stateFields: 更新状态字段名
     * @param state :  server状态
     *
     * @return server信息列表
     */
    int updateServerState(const string & app, const string & serverName, const string & nodeName,
            const string & stateFields, taf::ServerState state, int processId = -1);

    /**
     * 量批更新server状态
     *
     * @param vecStateInfo:       批量server的状态
     *
     * @return server信息列表
     */
	int updateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo);


    /**
     * 更新node的状态为inactive
     *
     * @param name    node名称
     *
     * @return 0-成功 others-失败
     */
    int destroyNode(const string & name);

    /** 根据id获取对象
     *
     * @param id 对象名称
     *
     * @return  返回所有该对象的活动endpoint列表
     */
    vector<EndpointF> findObjectById(const string & id);

    /** 根据id获取对象
     *
     * @param id 对象名称
     * @out param activeEp    存活的列表
     * @out param inactiveEp  非存活的列表
     *
     * @return 0-成功 others-失败
     */
    int findObjectById4All(const string & id,
                           vector<EndpointF>& activeEp,
                           vector<EndpointF>& inactiveEp);

     /** 根据id获取同组对象
     *
     * @param id 对象名称
     * @param ip
     * @out param activeEp    存活的列表
     * @out param inactiveEp  非存活的列表
     * @out param os          打印日志使用
     *
     * @return 0-成功 others-失败
     */
    int findObjectByIdInSameGroup(const string & id,
                                 const string & ip,
                                 vector<EndpointF>& activeEp,
                                 vector<EndpointF>& inactiveEp,
                                 ostringstream &os);

     /** 根据id获取优先级序列中的对象
     *
     * @param id 对象名称
     * @param ip
     * @out param activeEp    存活的列表
     * @out param inactiveEp  非存活的列表
     * @out param os          打印日志使用
     *
     * @return 0-成功 others-失败
     */
	int findObjectByIdInGroupPriority(const std::string &sID, const std::string &sIP, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

     /** 根据id和归属地获取全部对象
     *
	 * @param id 对象名称
	 * @param sStation 归属地
     * @out param activeEp    存活的列表
     * @out param inactiveEp  非存活的列表
     * @out param os          打印日志使用
     *
     * @return 0-成功 others-失败
     */
	int findObjectByIdInSameStation(const std::string &sID, const std::string & sStation, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

	int findObjectByIdInSameSet(const string &sID, const vector<string> &vtSetInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

	int findObjectByIdInSameSet(const string &sSetId, const map<string, vector<SetServerInfo> > &mpSetServerInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);
	/**
     * 获取application列表
     * @param null
     * @param out reuslt
     * @return application列表
     */
    vector<string> getAllApplicationNames(string & result);


    /**
     * 获取server列表
     * @param null
     * @return node 列表
     */
    vector<vector<string> > getAllServerIds(string & result);

    /**
     * 获取合并的配置文件
     * @param appServerName
     * @param fileName
     * @param host
     * @param config
     * @param resultDesc
     *
     * @return int
     */
    int getConfig(const string &appServerName, const string &fileName, const string &host, string &config, std::string &resultDesc);

    /**
     * 设置server发布版本
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     * @param version  : server 版本
     * @param user     : 发布者
     */
    int setPatchInfo(const string & app, const string & serverName, const string & nodeName,
            const string & version, const string & user);

    /**
     * 设置server的taf库版本
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     * @param version:  server基于的taf版本号
     *
     * @return 0-成功 others-失败
     */
    int setServerTafVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version);

    /**
     * 轮询数据库，将心跳超时的节点及server状态设为不存活
     * @param iTiemout 超时时间
     * @return
     */
    int checkNodeTimeout(unsigned uTimeout);

    /**
     * 轮询数据库，将心跳超时的registry设为不存活
     * @param iTiemout 超时时间
     * @return
     */
    int checkRegistryTimeout(unsigned uTimeout);

    /**
     * 定时检查在数据库的设置状态为“active”的服务在Node节点上的状态：
     * 如果服务在Node的设置状态不是“active”，则通知Node主动重启该服务
     *
     * @param iCheckTimeInterval 每次轮询最近更新的记录，单位为秒
     * @return
     */
    int checkSettingState(const int iCheckLeastChangedTime=3600);

    /**
     * 在加载对象列表之前，计算当前非活动状态的node的比率
     * @param NULL
     * @return
     */
	int computeInactiveRate();

    /**
     * 加载对象列表到内存
     * @param bRecoverProtect
     * @param iRecoverProtectRate
     * @param iLoadTimeInterval 加载最近iLoadTimeInterval秒内变化的记录
     * @param bLoadAll  是否加载所有服务
     *
     * @return
     */
    int loadObjectIdCache(const bool bRecoverProtect,
                            const int iRecoverProtectRate,
                            const int iLoadTimeInterval=60,
                            const bool bLoadAll=false);

    /**
     * 加载组优先级到内存
     * @param NULL
     * @return
     */
	int loadGroupPriority();

    /**
     * 更新registry信息到db
     */
    int updateRegistryInfo2Db(bool bRegHeartbeatOff=false);

    /**
     * 获取对应node的模板名称
     */
    int getNodeTemplateName(const string nodeName, string & sTemplateName);

    int initFileCache();

    /**
     * 根据ip获取组id
     * @return int <0 失败 其它正常
     */
    int getGroupId(const string& ip);

    /**
    *根据组名获取组id
    * @return int <0 失败 其它正常
    */
    int getGroupIdByName(const string& sGroupName);

    /**
     * 加载IP物理分组信息
    */
    int loadIPPhysicalGroupInfo();
protected:

    /**
     * 根据group id获取Endpoint
     */
    vector<EndpointF> getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, int iGroupId, ostringstream &os);

	vector<EndpointF> getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, const set<int> & setGroupID, ostringstream & os);


    /**
     * 更新分组信息到node表
     */
    int updateGroupId2DB();

    /**
     * updateServerStateBatch的底层实现函数
     */
	int doUpdateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, const size_t sizeBegin, const size_t sizeEnd);

protected:
    int dumpCache2HashMap(ObjectsCache & cache,const bool bLoadAll=false);
    int loadCacheFromHashMap(ObjectsCache & cache);
private:
	/**
	 * 更新缓存中的服务状态值
	 *
	 * @param mStatus
	 * @param updateAll 是否全部更新
	 */
	 void updateStatusCache(const std::map<std::string, int>& mStatus,bool updateAll=false);
	/**
	 * 更新缓存中的服务信息
	 *
	 * @param objCache
	 * @param updateAll 是否全部更新
	 */
	 void updateObjectsCache(const ObjectsCache& objCache,bool updateAll=false);

	/**
	 * 更新缓存中的set信息
	 *
	 * @param objCache
	 * @param updateAll 是否全部更新
	 */
	 void updateDivisionCache(const SetDivisionCache& setDivisionCache,bool updateAll=false);
	/**
	 * 对数据库查询结果执行联合操作
	 *
	 * @param data1
	 * @param data2
	 *
	 * @return 联合的结果
	 */
	 TC_Mysql::MysqlData UnionRecord(TC_Mysql::MysqlData& data1,TC_Mysql::MysqlData& data2);

protected:
    //mysql连接对象
    taf::TC_Mysql _mysqlReg;

    //node节点代理列表
    static map<string , NodePrx> _mapNodePrxCache;
    static TC_ThreadLock _NodePrxLock;

    //对象列表缓存
    static ObjectsCache _objectsCache[2];
    //正在使用的对象缓存下标
    static int _objectsFlag;

    //set划分缓存
    static SetDivisionCache _setDivisionCache[2];
    //正在使用的set缓存下标
    static int _setDivisionFlag;

	//优先级的序列
	static std::map<int, GroupPriorityEntry> _mapGroupPriority[2];
	static int _mapGroupPriorityFlag;

	//servant状态表
	static std::map<std::string, int> _mapServantStatus[2];
	static int _mapServantStatusFlag;

    //用于备份服务列表的hash
    static FileHashMap _fileCache;

    //用于初始化保护
    static TC_ThreadLock _mutex;

    //分组信息
    static map<string,int> _mServerGroupCache;
    static map<string,int> _mGroupNameIDCache;

    //匹配分组信息
    static vector< map<string,string> > _vServerGroupRule;

	//是否允许采用多条语句同时执行方式
	bool _bEnMultiSql;
};

#endif
