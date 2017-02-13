package com.qq.java.testserver;

import static com.qq.taf.server.ApplicationUtil.*;

import java.util.Map;

import com.qq.java.testserver.testservant.Send;
import com.qq.java.testserver.testservant.TestServantPrxCallback;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.server.JceCurrent;

public class TestServantImpl extends com.qq.java.testserver.testservant.TestServant {

	
	@Override
	protected String getCharacterSet() {
		return TestServer.ENCODE;
	}

	@Override
	public int testHello(String s, int i, long l, boolean b, byte c, byte[] vt, Map<String, String> mp, Send send, JceStringHolder r, JceCurrent _jc) {
		System.out.println("map=="+mp);
		r.value="wup3testok啊";
		return 0;
	}

	@Override
	public void testVoid(String s, int i, long l, boolean b, byte c, byte[] vt,
			Map<String, String> mp, Send send, JceStringHolder r, JceCurrent _jc) {
		System.out.println(s+"|"+i+"|"+l+"|"+b+"|"+c+"|"+vt+"|"+mp+"|"+send);
		r.value="testVoidok啊";
		
	}


}
