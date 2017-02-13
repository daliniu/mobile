package com.qq.java.testclient;

import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qq.java.testserver.TestServer;
import com.qq.java.testserver.testservant.Send;
import com.qq.java.testserver.testservant.TestServantPrx;
import com.qq.java.testserver.testservant.TestServantPrxCallback;
import com.qq.java.testserver.testservant.TestServantPrxHelper;
import com.qq.taf.EndpointF;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxy;

public class TestProxy {
	
	private static void testStruct(String[] args)throws Throwable {
		Send s=new Send();
		JceOutputStream jos=new JceOutputStream();
		s.writeTo(jos);
		byte[] data=jos.toByteArray();
		JceInputStream jis=new JceInputStream(data);
		Send ss=new Send();
		ss.readFrom(jis);
		System.out.println(ss);
	}
	
	private static void testGetEndpoint4All(String[] args)throws Throwable {
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
	
	private static void testAsyncVsSync(String args[], Communicator comm, String servantName) throws Throwable {
		TestServantPrx prx=comm.stringToProxy(servantName, TestServantPrxHelper.class);
		((TestServantPrxHelper)prx).setServerEncoding(TestServer.ENCODE);
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
			TestServantPrxCallback callback=new TestServantPrxCallback() {
				@Override
				public void callback_testHello_exception(int _iRet) {
				}
				@Override
				public void callback_testHello(int _ret, String r) {
					System.out.println(ii+"|"+(System.currentTimeMillis()-begin));
				}
				@Override
				public void callback_testVoid(String r) {
					// TODO Auto-generated method stub
					
				}
				@Override
				public void callback_testVoid_exception(int _iRet) {
					// TODO Auto-generated method stub
					
				}
			};
			prx.async_testHello(callback,"a", 0, 1L, true, (byte)3, vt, mp, send);
			
		}
	}

	public static void testOneWay(Communicator comm, String servantName)throws Throwable {
		TestServantPrx prx=comm.stringToProxy(servantName, TestServantPrxHelper.class);
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		for(int i=0;i<100;i++) {
			prx.async_testHello(null, "a", 0, 1L, true, (byte)3, vt, mp, send);
		}
		System.out.println("ok");
	}
	
	public static void testHash(Communicator comm, String servantName) throws InterruptedException {
		System.out.println("======begin test hash======");
		TestServantPrx prx=comm.stringToProxy(servantName, TestServantPrxHelper.class);
		((TestServantPrxHelper)prx).setServerEncoding(TestServer.ENCODE);
		byte[] vt=new byte[]{10,20,30};
		Map<String, String> mp=new HashMap<String, String>();
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
		Send send=new Send();
		JceStringHolder r=new JceStringHolder();
		for(int i=0;i<100;i++) {
			System.out.println(i+" hash return "+((TestServantPrx)((ServantProxy)prx).taf_hash(i)).testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r)+"|r.value="+r.value);
		}
		System.out.println("=============hash ok================");
	}
	
	public static void main(String[] args) throws Throwable {
		try {
			String conf=TestProxy.class.getResource("/client.conf").getFile();
			String servantName = System.getProperty("servantName", "Java.TestServer.TestServant");
			Communicator comm=new Communicator(conf); 
			testHash(comm, servantName);
			testGetEndpoint4All(args);
			testAsyncVsSync(args, comm, servantName);
			testStruct(args);
			testOneWay(comm, servantName);
			testHash(comm, servantName);
			System.out.println("begin sleep 3 min "+new Date());
			Thread.sleep(1000*60*3);
			System.out.println("end sleep 3 min "+new Date());
			testHash(comm, servantName);
			testGetEndpoint4All(args);
			testAsyncVsSync(args, comm, servantName);
			testStruct(args);
			testOneWay(comm, servantName);
		}
		catch(Throwable t) {
			System.err.println("oh no");
		}
	}
	
}
