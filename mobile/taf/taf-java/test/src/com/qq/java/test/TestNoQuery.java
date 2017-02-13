package com.qq.java.test;

import java.util.HashMap;
import java.util.Map;

import com.qq.java.test.basic.BasicPrx;
import com.qq.java.test.basic.BasicPrxHelper;
import com.qq.java.test.basic.Send;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;

public class TestNoQuery {
	public static void main(String ...args) throws Throwable {
		Communicator comm = new Communicator(TestClient.class.getResource("/client.conf").getFile());
		BasicPrx basic = (BasicPrx)comm.stringToProxy("Java.Test.Basic@tcp -h 10.130.64.220 -p 12736", BasicPrxHelper.class);
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		for(int i=0;i<100;i++) {
			System.out.println(basic.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r)+"|"+i);
		}
		Thread.sleep(1000*60*2);
		for(int i=0;i<100;i++) {
			System.out.println(basic.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r)+"|"+i);
		}
	}
}
