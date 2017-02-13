package com.duowan.taf.proxy;

import com.duowan.taf.ResponsePacket;
import com.duowan.taf.proxy.ServantProxyThreadData.ThreadPrivateData;

/**
 * 客户端异步调用的回调接口的基类
 * @author fanzhang
 */
public abstract class ServantProxyCallback implements CallbackHandler {
	
	public ThreadPrivateData _data;
	
	public abstract int _onDispatch(String sFuncName, ResponsePacket response);

}
