package com.qq.java.test;

import java.util.Map;

import com.qq.java.test.basic.Send;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.server.JceCurrent;

public class BasicImpl extends com.qq.java.test.basic.Basic {

	@Override
	public int testHello(String s, int i, long l, boolean b, byte c, byte[] vt, Map<String, String> mp, Send send, JceStringHolder r, JceCurrent _jc) {
		r.value="OK|"+s+"|"+i+"|"+l+"|"+b+"|"+c+"|"+vt.length+"|"+mp.size()+"|"+send.toString();
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return 0;
	}
	
	
}
