package com.qq.taf.proxy.test.router;

import java.util.concurrent.atomic.AtomicInteger;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.test.BechmarkFrameWork;
import com.qq.taf.proxy.test.router.RouterTest.Router;
import com.qq.taf.proxy.test.router.Test.HelloPrxCallback;
import com.qq.taf.proxy.test.router.Test.HelloPrxHelper;

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
public class RouterBechmark extends BechmarkFrameWork {

	static int testThreadCount = 1;
	static int testMinute = 1;
	static int monitorInterv = 6000;
	static String objectName = "Test.HelloRouteServer.HelloRouteObj";
	static int testMethod = 0;
	

	static Communicator c = null;
	static HelloPrxHelper helper = null;
	Task task = new Task(helper);
	
	AtomicInteger result = new AtomicInteger();

	class Task extends HelloPrxCallback implements Runnable {
		HelloPrxHelper helper = null;

		public Task(HelloPrxHelper helper) {
			this.helper = helper;
		}

		public void run() {
			while (isRunning) {
				try {
					String si = "si";
					if (testMethod == 0) {
						JceStringHolder holer = new JceStringHolder();
						int returnInt = helper.testHello(si, holer);
						if ( returnInt == 0 ) {
							result.incrementAndGet();
						}
						completeIncrementAndGet();
					} else {
						helper.async_testHello(this, si);
						writeCountIncrementAndGet();
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					ErrorIncrementAndGet();
				}

			}
		}

		@Override
		public void callback_testHello(int _ret, String r) {
			completeIncrementAndGet();			
			if ( _ret == 0  ) {
				result.incrementAndGet();
			}
		}

		@Override
		public void callback_testHello_exception(int ret) {
			ErrorIncrementAndGet();			
		}

		
		
	}

	public void benchmarkFinished() {
		System.out.println("result:"+result.get());
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			c = new Communicator("client.config.conf");
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
			helper = c.stringToProxy(objectName, HelloPrxHelper.class);
			//helper.taf_set_router(new Router());
			RouterBechmark b = new RouterBechmark();
			b.startTest();
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
	
	@Override
	public String getMonitorPrint() {
		return " sbs:"+c.getSendBufferSize()+" rbs:"+c.getReceiveBufferSize();
	}

}
