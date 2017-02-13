package com.qq.taf.proxy.test;

import java.util.HashMap;
import java.util.Map;

import com.qq.netutil.nio.LoggerCenter;
import com.qq.netutil.nio.mina2.util.SmallUtil;
import com.qq.taf.proxy.test.Bechmark;
import com.qq.taf.proxy.test.BechmarkFrameWork;
import com.qq.taf.proxy.test.Test.*;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.CommunicatorConfig;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.conn.LoggerFilter;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;

/**
 * mqq@31_54_3g:~/taf/onenode/data/servers/Comm.BindServer/bin>
 /usr/local/app/taf/onenode/data/servers/Comm.BindServer/bin/BindServer
 --config
 =/usr/local/app/taf/onenode/data/servers/Comm.BindServer/conf/BindServer.config.conf 
mqq@31_48_3g:~/onetest/classes> 
java -Xmx512m -cp /usr/local/app/onetest/classes:/usr/local/app/onetest/javalib/netTool-1.0.0.jar:/usr/local/app/onetest/javalib/jutil-1.0.2.jar
 com.qq.taf.proxy.simpletest.Bechmark
 * 
 * @author albertzhu
 * 
 */
public class Bechmark extends BechmarkFrameWork {

	static int testThreadCount = 1;
	static int testMinute = 1;
	static int monitorInterv = 6000;
	static String objectName = "Comm.BindServer.BindObj";
	static int testMethod = 0;
	
	static String registerProtocol = "tcp";
	static String registerHost = "127.0.0.1";
	static int registerPort = 17890;
	//static int registerPort = 17890;
	//static int registerTimeoutSe = 3;

	static Communicator c = null;
	static TestBasePrx helper = null;
	Task task = new Task(helper);

	class Task extends TestBasePrxCallback implements Runnable {
		TestBasePrx helper = null;

		public Task(TestBasePrx helper) {
			this.helper = helper;
		}

		public void run() {
			while (isRunning) {
				try {
					int id = 1;
					String si = "si";
					if (testMethod == 0) {
						JceStringHolder so = new JceStringHolder("so");
						int returnInt = helper.testStr(id, si, so);
						completeIncrementAndGet();
					} else {
						helper.async_testStr(this, id, si);
						writeCountIncrementAndGet();
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					ErrorIncrementAndGet();
				}

			}
		}

		@Override
		public void callback_testMapByte(int _ret, Map<Byte, String> mo) {
			System.out.println("callback_testMapByte "+mo);
		}

		@Override
		public void callback_testMapByte_exception(int ret) {
			System.out.println("callback_testMapByte_exception "+ret);
			LoggerCenter.info("callback_testMapByte_exception "+ret);
		}

		@Override
		public void callback_testMapInt(int _ret, Map<Integer, Integer> mo) {
			System.out.println("callback_testMapInt "+mo);
		}

		@Override
		public void callback_testMapIntDouble(int _ret, Map<Integer, Double> mo) {
			System.out.println("callback_testMapIntDouble "+mo);
		}

		@Override
		public void callback_testMapIntDouble_exception(int ret) {
			System.out.println("callback_testMapIntDouble_exception "+ret);
			LoggerCenter.info("callback_testMapIntDouble_exception "+ret);
		}

		@Override
		public void callback_testMapIntFloat(int _ret, Map<Integer, Float> mo) {
			System.out.println("callback_testMapIntFloat "+mo);
		}

		@Override
		public void callback_testMapIntFloat_exception(int ret) {
			System.out.println("callback_testMapIntFloat_exception "+ret);
			LoggerCenter.info("callback_testMapIntFloat_exception "+ret);
		}

		@Override
		public void callback_testMapIntStr(int _ret, Map<Integer, String> mo) {
			System.out.println("callback_testMapIntStr "+mo);
		}

		@Override
		public void callback_testMapIntStr_exception(int ret) {
			System.out.println("callback_testMapIntStr_exception "+ret);
			LoggerCenter.info("callback_testMapIntStr_exception "+ret);
		}

		@Override
		public void callback_testMapInt_exception(int ret) {
			System.out.println("callback_testMapInt_exception "+ret);
			LoggerCenter.info("callback_testMapInt_exception "+ret);
		}

		@Override
		public void callback_testMapStr(int _ret, Map<String, String> mo) {
			System.out.println("callback_testMapStr "+mo);
		}

		@Override
		public void callback_testMapStr_exception(int ret) {
			System.out.println("callback_testMapStr_exception "+ret);
			LoggerCenter.info("callback_testMapStr_exception "+ret);
		}

		@Override
		public void callback_testStr(int _ret, String so) {
			System.out.println("callback_testStr "+so);
		}

		@Override
		public void callback_testStr_exception(int ret) {
			System.out.println("callback_testStr_exception "+ret);
			LoggerCenter.info("callback_testStr_exception "+ret);
		}


		@Override
		public void callback_testVectorByte_exception(int ret) {
			System.out.println("callback_testVectorByte_exception "+ret);
			LoggerCenter.info("callback_testVectorByte_exception "+ret);
		}


		@Override
		public void callback_testVectorDouble_exception(int ret) {
			System.out.println("callback_testVectorDouble_exception "+ret);
			LoggerCenter.info("callback_testVectorDouble_exception "+ret);
		}


		@Override
		public void callback_testVectorFloat_exception(int ret) {
			System.out.println("callback_testVectorFloat_exception "+ret);
			LoggerCenter.info("callback_testVectorFloat_exception "+ret);
		}


		@Override
		public void callback_testVectorInt_exception(int ret) {
			System.out.println("callback_testVectorInt_exception "+ret);
			LoggerCenter.info("callback_testVectorInt_exception "+ret);
		}


		@Override
		public void callback_testVectorShort_exception(int ret) {
			System.out.println("callback_testVectorShort_exception "+ret);
			LoggerCenter.info("callback_testVectorShort_exception "+ret);
		}


		@Override
		public void callback_testVectorStr_exception(int ret) {
			System.out.println("callback_testVectorStr_exception "+ret);
			LoggerCenter.info("callback_testVectorStr_exception "+ret);
		}

		@Override
		public void callback_testVectorByte(int _ret, byte[] vo) {
			System.out.println("callback_testVectorByte "+vo);			
		}

		@Override
		public void callback_testVectorDouble(int _ret, double[] vo) {
			System.out.println("callback_testVectorDouble "+vo);			
		}

		@Override
		public void callback_testVectorFloat(int _ret, float[] vo) {
			System.out.println("callback_testVectorFloat "+vo);
		}

		@Override
		public void callback_testVectorInt(int _ret, int[] vo) {
			System.out.println("callback_testVectorInt "+vo);
		}

		@Override
		public void callback_testVectorShort(int _ret, short[] vo) {
			System.out.println("callback_testVectorShort "+vo);
		}

		@Override
		public void callback_testVectorStr(int _ret, String[] vo) {
			System.out.println("callback_testVectorStr "+vo);
		}

		
	}

	public void benchmarkFinished() {

	}
	
	public static void testBenchService() {
		Communicator comm = new Communicator();
		comm.setMaxInvokeTimeout(5000);
		comm.setModulename("taf.onetest");
		objectName = objectName+"@tcp -h 127.0.0.1 -p 22223:tcp -h 127.0.0.1 -p 22224";
		helper = comm.stringToProxy(objectName, TestBasePrx.class);
		helper.taf_async_timeout(6000);
		int id = 0 ;
		String si = "1" ;
		JceStringHolder so = new JceStringHolder("2") ;
		for ( int i = 0 ; i < 1000 ; i++ ) {
			try {
				System.out.println("----------------------received:"+helper.testStr(id, si, so));
			} catch (Exception e1) {
				e1.printStackTrace();
			}
			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
	}
	
	public static void testFull(String[] args) {
		// ProxyConfig config =
		// ProxyConfig.parseConfig("client.config.conf");
		// c = new Communicator(config);
		// helper = c.stringToProxy(objectName, TestServiceHelper.class);

		LoggerFilter.addCareServer("tcp", "127.0.0.1", 23333);
		//LoggerFliter.addCareServer("tcp", "127.0.0.1", 17890);
		//LoggerFliter.addCareServer("tcp", "127.0.0.1", 9985);
		c = new Communicator();
		c.setLocator(registerProtocol + " -h " + registerHost + " -p " + registerPort);
		c.setStat("taf.onestat.StatObj");
		c.setMaxInvokeTimeout(5000);
		c.setModulename("taf.onetest");
		
		if (args.length > 0) {
			testThreadCount = Integer.parseInt(args[0]);
		}
		if (args.length > 1) {
			testMinute = Integer.parseInt(args[1]);
		}
		if (args.length > 2) {
			testMethod = Integer.parseInt(args[2]);
		}
		if (args.length > 3) {
			monitorInterv = Integer.parseInt(args[3]) * 1000;
		}
		if (args.length > 4) {
			objectName = args[4];
		}
		//ProxyConfig bindObjConfig = new ProxyConfig(objectName);
		//bindObjConfig.nodeList.add(new ClientConnConfig("tcp","10.1.36.39" ,22224 , 2));
		//EndPointConfig cc = new EndPointConfig("tcp", host, port, 2);
		//cc.setAllowFutureCount(40);
		//bindObjConfig.nodeList.add(cc);
		helper = c.stringToProxy(objectName, TestBasePrx.class);
		helper.taf_async_timeout(6000);
		Bechmark b = new Bechmark();
		b.startTest();
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			//testFull(args);
			testBenchService();
			
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public int getMonitorInterv() {
		return monitorInterv;
	}

	@Override
	public Runnable getTestTask() {
		return task;
	}

	@Override
	public int getTestThreadCount() {
		return testThreadCount;
	}

	@Override
	public int getTestMinute() {
		return testMinute;
	}

}
