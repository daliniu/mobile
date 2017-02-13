package com.qq.java.test;

import java.util.HashMap;
import java.util.Map;

import com.qq.java.test.basic.BasicPrx;
import com.qq.java.test.basic.BasicPrxCallback;
import com.qq.java.test.basic.BasicPrxHelper;
import com.qq.java.test.basic.Send;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;

public class TestStat {
	
	private static void testAsyncVsSync(Communicator comm, String servantName) throws Throwable {
		BasicPrx prx=comm.stringToProxy(servantName, BasicPrxHelper.class);
		((BasicPrxHelper)prx).setServerEncoding("GBK");
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100埃克斯");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		
		System.out.println("=============== Sync =================");
		for(int i=0;i<100;i++) {
			long begin=System.currentTimeMillis();
			prx.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r);
			
			System.out.println(i+"|"+(System.currentTimeMillis()-begin)+"|r.value="+r.value+"|");
		}
		
		System.out.println("=============== Async =================");
		for(int i=0;i<100;i++) {
			final long begin=System.currentTimeMillis();
			final int ii=i;
			BasicPrxCallback callback=new BasicPrxCallback() {
				@Override
				public void callback_testHello_exception(int _iRet) {
				}
				@Override
				public void callback_testHello(int _ret, String r) {
					System.out.println(ii+"|"+(System.currentTimeMillis()-begin));
				}
			};
			prx.async_testHello(callback,"a", 0, 1L, true, (byte)3, vt, mp, send);
			
		}
		System.out.println("testAsyncVsSync ok");
	}

	public static void testOneWay(Communicator comm, String servantName)throws Throwable {
		BasicPrx prx=comm.stringToProxy(servantName, BasicPrxHelper.class);
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		for(int i=0;i<100;i++) {
			prx.async_testHello(null, "a", 0, 1L, true, (byte)3, vt, mp, send);
		}
		System.out.println("testOneWay ok");
	}
	
	public static void main(String...args) throws Throwable{
		String conf=TestStat.class.getResource("/client.conf").getFile();
		String servantName = System.getProperty("servantName", "Java.Test.Basic");
		Communicator comm=new Communicator(conf); 
		testAsyncVsSync(comm, servantName);
		testOneWay(comm, servantName);
	}
}
