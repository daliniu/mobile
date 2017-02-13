#ifndef __QUERY_IMP_H__
#define __QUERY_IMP_H__

#include "QueryF.h"
#include "DbHandle.h"
#include "RouterProxyCallback.h"

using namespace taf;

/**
 * 对象查询接口类
 */

class QueryImp: public QueryF
{
public:
    /**
     * 构造函数
     */
    QueryImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};


    /** 根据id获取对象
     *
     * @param id 对象名称
     *
     * @return  返回所有该对象的活动endpoint列表
     */
    virtual vector<EndpointF> findObjectById(const string & id, taf::JceCurrentPtr current);

	/**根据id获取所有对象,包括活动和非活动对象
	 *
     * @param id         对象名称
     * @param activeEp   存活endpoint列表
     * @param inactiveEp 非存活endpoint列表
     * @return:  0-成功  others-失败
     */
    virtual taf::Int32 findObjectById4Any(const std::string & id,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

    /** 根据id获取对象所有endpoint列表
     *
     * @param id         对象名称
     * @param activeEp   存活endpoint列表
     * @param inactiveEp 非存活endpoint列表
     * @return:  0-成功  others-失败
     */
    Int32 findObjectById4All(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

     /** 根据id获取对象同组endpoint列表
     *
     * @param id         对象名称
     * @param activeEp   存活endpoint列表
     * @param inactiveEp 非存活endpoint列表
     * @return:  0-成功  others-失败
     */
    Int32 findObjectByIdInSameGroup(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

	/** 根据id获取对象指定归属地的endpoint列表
	 *
	 * @param id         对象名称
	 * @param activeEp   存活endpoint列表
	 * @param inactiveEp 非存活endpoint列表
     * @return:  0-成功  others-失败
     */
	Int32 findObjectByIdInSameStation(const std::string & id, const std::string & sStation, vector<taf::EndpointF> &activeEp, vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current);

	/** 根据id获取对象同set endpoint列表
	*
	* @param id         对象名称
	* @param setId      set全称,格式为setname.setarea.setgroup
	* @param activeEp   存活endpoint列表
	* @param inactiveEp 非存活endpoint列表
	* @return:  0-成功  others-失败
	*/
	Int32 findObjectByIdInSameSet(const std::string & id,const std::string & setId,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current);

private:
	/**
	 * 判断请求的id是否为taserver的服务
	 */
	bool IsTaServerObjById(const std::string & id);
	/**
	* @param id        taserver对象名称
	* @param eFnId     请求的接口名称枚举，详情请参考FUNID
	* @param current   请求的上下文
	* @return:  0-成功  others-失败
	 */
	int findTaServerObjById(const std::string & id,const FUNID eFnId,const taf::JceCurrentPtr& current);

    void doDaylog(const string& id,const vector<taf::EndpointF> &activeEp, const vector<taf::EndpointF> &inactiveEp, const taf::JceCurrentPtr& current,const std::ostringstream& os,const string& sSetid="");
protected:
    //数据库操作
    CDbHandle _db;

};

#endif
