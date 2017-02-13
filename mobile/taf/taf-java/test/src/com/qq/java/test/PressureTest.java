package com.qq.java.test;

import java.util.HashMap;
import java.util.Map;

import com.qq.java.test.basic.BasicPrx;
import com.qq.java.test.basic.BasicPrxCallback;
import com.qq.java.test.basic.BasicPrxHelper;
import com.qq.java.test.basic.Send;
import com.qq.jutil.j4log.Logger;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;

public class PressureTest {
	private static Communicator comm;
	private static Logger log = Logger.getLogger("test");
	
	private static int time = 0;
	private static long tend = 0;
	private static long tbegin = 0;
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
	
	private static byte[] vt=new byte[]{10,20,30};
	private static Map<String, String> mp=new HashMap<String, String>();
	static {
		mp.put("x", "100");
		mp.put("y", "200");
		mp.put("z", "300");
	}
	private static Send send=new Send();
	static JceStringHolder r=new JceStringHolder();
	static int count = Integer.valueOf(System.getProperty("count", "1000000"));
	private static BasicPrxCallback callback = new BasicPrxCallback() {

		@Override
		public void callback_testHello(int _ret, String r) {
			if(++time==count){
				tend = System.currentTimeMillis();
				System.out.println("async cost|"+(tend-tbegin));
			}
			else if((time&8095)==0) {
				log.info("i=="+time);
			}
		}

		@Override
		public void callback_testHello_exception(int _iRet) {
			System.out.println("exception found");
		}
	};
	
	static class MultTester extends Thread {
		public void run() {
			
				boolean isSync = Boolean.parseBoolean(System.getProperty("sync", "true"));
				int count = Integer.valueOf(System.getProperty("count", "1000000"));
				int sleep = Integer.valueOf(System.getProperty("sleep", "1000"));
				long begin = System.currentTimeMillis();
				log.info(getName()+"|begin|"+begin);
				if(isSync) {
					for(int i=0;i<count;i++) {
						try {
							basic.testHello("a", 0, 1L, true, (byte)3, vt, mp, send, r);
							if(sleep >0) {
								Thread.sleep(sleep);
							}
							if((i&8191) == 0) {
								log.info("i=="+i);
							}
						}
						catch(Throwable th) {
							th.printStackTrace();
						}
					}
					log.info(getName()+"|sync cost|"+(System.currentTimeMillis()-begin));
				}
				else {
					tbegin = System.currentTimeMillis();
					for(int i=0;i<count;i++) {
						try {
							basic.async_testHello(callback,"a",  0, 1L, true, (byte)3, vt, mp, send);
							if(i%100==0) {
								sleep(2);
							}
						}
						catch(Throwable th) {
							th.printStackTrace();
						}
					}
				}
			}
		}
	public static void main(String ...args) throws Throwable {
		int threads = Integer.parseInt(System.getProperty("threads", "1"));
		MultTester[] tester = new MultTester[threads];
		for(int i=0;i<tester.length;i++) {
			tester[i] = new MultTester();
			tester[i].setName("Tester-"+i);
		}
		for(int i=0;i<tester.length;i++) {
			tester[i].start();
		}
	}
}
