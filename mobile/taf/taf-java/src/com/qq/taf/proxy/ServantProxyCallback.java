package com.qq.taf.proxy;

import com.qq.taf.ResponsePacket;
import com.qq.taf.proxy.ServantProxyThreadData.ThreadPrivateData;

/**
 * 客户端异步调用的回调接口的基类
 * @author fanzhang
 */
public abstract class ServantProxyCallback implements CallbackHandler {
	
	public ThreadPrivateData _data;
	
	public abstract int _onDispatch(String sFuncName, ResponsePacket response);

}
