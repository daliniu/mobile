package com.qq.java.test;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qq.java.test.basic.BasicPrx;
import com.qq.java.test.basic.BasicPrxHelper;
import com.qq.java.test.basic.Send;
import com.qq.taf.EndpointF;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxy;

public class TestClient {
	private static Communicator comm;
	private static BasicPrx basic ;
	static {
		try {
			comm = new Communicator(TestClient.class.getResource("/client.conf").getFile());
			basic=comm.stringToProxy("Java.Test.Basic", BasicPrxHelper.class);
		}
		catch(Throwable t) {
			t.printStackTrace();
		}
	}
	public static void testOneWay() {
		//System.out.println("== test OneWay.");
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		basic.async_testHello(null, "a", 0, 1L, true, (byte)3, vt, mp, send);
		//int ret=basic.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r);
		//System.out.println("ret:"+ret);
		//System.out.println("r:"+r.value);
	}
	
	public static void test() {
		//System.out.println("== test client.");
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		int ret=basic.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r);
		if(ret != 0) {
			System.err.println("return "+ret);
		}
		//System.out.println("ret:"+ret);
		//System.out.println("r:"+r.value);
	}
	
	private static void testGetEndpoint4All(String[] args) {
		System.out.println("==========Begin============");
		String configFile=args[0];
		String objectName=args[1];
		Communicator communicator=new Communicator(configFile);
		List<EndpointF> endpoints=communicator.getEndpoint4All(objectName);
		System.out.println("======"+endpoints.size()+"======");
		for(EndpointF sepi:endpoints) 
			System.out.println("======="+sepi.host+":"+sepi.port+" real groupid=="+sepi.grouprealid+" work groupid=="+sepi.groupworkid+"======");
		System.out.println("===========End===========");
	}
	
	public static void testHash(Communicator comm, String servantName) throws InterruptedException {
		System.out.println("======begin test hash======");
		BasicPrx prx=comm.stringToProxy(servantName, BasicPrxHelper.class);
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		for(int i=0;i<100;i++) {
			System.out.println(i+" hash return "+((BasicPrx)((ServantProxy)prx).taf_hash(i)).testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r));
		}
		System.out.println("=============hash ok================");
	}
	
	public static void main(String[] args) throws Throwable {
		test();
		testOneWay();
		//testGetEndpoint4All(args);
		testHash(comm, "Java.Test.Basic");
		System.out.println("begin to sleep 3 min");
		Thread.sleep(1000*60*3);
		System.out.println("end to sleep 3 min");
		test();
		testOneWay();
		//testGetEndpoint4All(args);
		testHash(comm, "Java.Test.Basic");
	}

	
}
