package com.qq.taf.proxy;

public interface ServantInterface {

	/**
	 * 设置hash值
	 * 
	 * @param hash
	 */
	public abstract ServantInterface taf_hash(int hash);

	/**
	 * 获取hash值
	 * 
	 * @return
	 */
	public abstract int taf_hash();

	/**
	 * 设置选择器
	 * 
	 * @param selector
	 */
	public abstract void taf_tAdapterSelector(TAdapterSelector selector);

	/**
	 * 获取选择器
	 * 
	 * @return
	 */
	public abstract TAdapterSelector taf_tAdapterSelector();
	
	/**
	 * 设置proxy从locator查询的间隔 默认是1000*60
	 * 
	 * @param millSecond
	 */
	public abstract void taf_referNodeInterv(int millSecond);
	
	/**
	 * 获取proxy从locator查询的间隔 默认是1000*60
	 * 
	 * @param millSecond
	 */
	public abstract long taf_referNodeInterv();

	/**
	 * 设置同步超时时间
	 * 
	 * @param millSecond
	 */
	public abstract void taf_sync_timeout(int millSecond);

	/**
	 * 获取同步超时时间
	 * 
	 * @return
	 */
	public abstract int taf_sync_timeout();

	/**
	 * 设置异步超时时间
	 * 
	 * @param millSecond
	 */
	public abstract void taf_async_timeout(int millSecond);

	/**
	 * 获取异步超时时间
	 * 
	 * @return
	 */
	public abstract int taf_async_timeout();

	/**
	 * 设置刷新endPoint列表时间
	 * 
	 * @param referMillSecond
	 */
	public abstract void taf_refreshEndPInterval(int referMillSecond);

	/**
	 * 获取刷新时间
	 * 
	 * @return
	 */
	public abstract int taf_refreshEndPInterval();

	/**
	 * 设置灰度Router
	 * 
	 * @return
	 */
	public abstract void taf_set_router(ServantProxyRouter router);

	/**
	 * 获取灰度Router
	 * 
	 * @return
	 */
	public abstract ServantProxyRouter taf_Router();

	public int taf_readBufferSize();

	public void taf_readBufferSize(int readBufferSize);

	public int taf_writeBufferSize();

	public void taf_writeBufferSize(int writeBufferSize);
	/**
	 * 网络连接在此时间内没有操作就会被关闭 时间为毫秒 必须大于30秒
	 * @return
	 */
	public long taf_idleStatusInteval();
	/**
	 * 网络连接在此时间内没有操作就会被关闭 
	 * @param idleStatusInteval 时间为毫秒 必须大于30秒
	 */
	public void taf_idleStatusInteval(long idleStatusInteval);

	 /**
     * 设置超时检查参数
     * 计算到某台服务器的超时率, 如果连续超时次数或者超时比例超过阀值
     * 默认60s内, 超时调用次数>=2, 超时比率0.7或者连续超时次数>500则失效
     * 服务失效后, 请求将尽可能的切换到其他可能的服务器, 并每隔tryTimeInterval尝试一次, 如果成功则认为恢复
     * 如果其他服务器都失效, 则随机选择一台尝试
	 * 如果是灰度状态的服务, 服务失效后如果请求切换, 也只会转发到相同灰度状态的服务
     * @int minTimeoutInvoke, 计算的最小的超时次数, 默认2次(在checkTimeoutInterval时间内超过了minTimeoutInvoke, 才计算超时)
     * @int frequenceFailInvoke, 连续失败次数
     * @int checkTimeoutInterval, 统计时间间隔, (默认60s, 不能小于30s)
     * @float radio, 超时比例 > 该值则认为超时了 ( 0.1<=radio<=1.0 )
	 * @int tryTimeInterval, 重试时间间隔
     */
	public void taf_check_timeout(int minTimeoutInvoke, int checkTimeoutInterval, int frequenceFailInvoke, float radio,
			int tryTimeInterval);
	
	public void taf_min_timeout_invoke(int minTimeoutInvoke);

	public int taf_min_timeout_invoke();

	public void taf_timeout_interval(int checkTimeoutInterval);

	public int taf_timeout_interval();

	public void taf_frequence_fail_invoke(int frequenceFailInvoke);

	public int taf_frequence_fail_invoke();

	public void taf_timeout_radio(float radio);

	public float taf_timeout_radio();

	public void taf_try_time_interval(int tryTimeInterval);

	public int taf_try_time_interval();
	
	public String taf_proxyName();
	
	public void taf_proxyName(String name);

}