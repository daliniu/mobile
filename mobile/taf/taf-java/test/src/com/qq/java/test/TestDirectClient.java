package com.qq.java.test;

import java.util.HashMap;
import java.util.Map;

import com.qq.java.test.basic.BasicPrx;
import com.qq.java.test.basic.BasicPrxHelper;
import com.qq.java.test.basic.Send;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxy;

public class TestDirectClient {
	private static Communicator comm;
	private static BasicPrx basic ;
	static {
		try {
			comm = new Communicator(TestClient.class.getResource("/client.conf").getFile());
			basic=comm.stringToProxy("Java.Test.Basic@tcp -h 10.130.64.220 -t 5000 -p 12736", BasicPrxHelper.class);
		}
		catch(Throwable t) {
			t.printStackTrace();
		}
	}
	
	public static void main(String ...args) {
		System.out.println("======begin test hash======");
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		for(int i=0;i<100;i++) {
			try {
				Thread.sleep(3000);
				System.out.println(i+" hash return "+(basic).testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r));
			}
			catch(Throwable th) {
				th.printStackTrace();
			}
		}
		System.out.println("=============hash ok================");
	}
}
