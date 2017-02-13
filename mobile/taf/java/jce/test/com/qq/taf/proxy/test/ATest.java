package com.qq.taf.proxy.test;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.CommunicatorConfig;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.test.Test.APrx;
import com.qq.taf.proxy.test.Test.APrxCallback;

public class ATest extends BechmarkFrameWork{

	static APrx aprx = null ;
	static Communicator c ;
	
	static String registerProtocol = "tcp";
	static String registerHost = "127.0.0.1";
	static int registerPort = 17890;
	
	static int testThreadCount = 1;
	static int testMinute = 1;
	static int monitorInterv = 6000;
	static String objectName = "Comm.BindServer.BindObj";
	static int testMethod = 0;
	Task task = new Task(aprx);
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		c = new Communicator();
		//c.setProperty(CommunicatorConfig.locator_Key, registerProtocol + " -h " + registerHost + " -p " + registerPort);
		//c.setStat("taf.onestat.StatObj");
		//c.setMaxInvokeTimeout(5000);
		//c.setModulename("taf.onetest");
		
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
		aprx = c.stringToProxy(objectName, APrx.class);
		aprx.taf_async_timeout(6000);
		ATest b = new ATest();
		b.startTest();
	}
	
	class Task extends APrxCallback implements Runnable {
		APrx aprx = null;

		public Task(APrx aprx) {
			this.aprx = aprx;
		}

		public void run() {
			while (isRunning) {
				try {
					long l = 1 ;
					int i = 1 ;
					String si = "si";
					if (testMethod == 0) {
						JceStringHolder so = new JceStringHolder("so");
						int returnInt = aprx.test(l, i);
						completeIncrementAndGet();
					} else {
						aprx.async_test(this, l, i);
						writeCountIncrementAndGet();
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					ErrorIncrementAndGet();
				}

			}
		}

		@Override
		public void callback_test(int _ret) {
			completeIncrementAndGet();
		}

		@Override
		public void callback_test_exception(int ret) {
			System.out.println("callback_test_exception:"+ret);
			ErrorIncrementAndGet();			
		}

	}

	public void benchmarkFinished() {

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
