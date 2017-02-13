package com.duowan.taf.proxy;

import com.duowan.taf.ResponsePacket;

/**
 * 异步回调接口
 * @author albertzhu
 *
 */
public interface CallbackHandler {
	int _onDispatch(String sFuncName, ResponsePacket response );
}
