package com.qq.taf.proxy;

import com.qq.tac2.jdt.share.AsyncClientException;
import com.qq.tac2.jdt.share.AsyncClientInfoBox;
import com.qq.taf.ResponsePacket;
import com.qq.taf.proxy.exec.TafException;

public class TacCallbackHandler extends ServantProxyCallback {

	AsyncClientInfoBox __box__;

	public TacCallbackHandler(com.qq.tac2.jdt.share.AsyncClientInfoBox __box__) {
		this.__box__ = __box__;
	}

	public int _onDispatch(String sFuncName, ResponsePacket response) {
		try {
			if (response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value) {
				__box__.setException(new AsyncClientException(TafException.makeException(response.iRet)));

			} else {
				__box__.setResult(response.sBuffer);
			}
		} finally {
			__box__.resumeRequest();
		}
		return response.iRet;
	}

}
