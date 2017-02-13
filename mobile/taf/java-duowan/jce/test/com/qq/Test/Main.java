package com.qq.Test;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.CommunicatorFactory;

public class Main {

	Communicator comm ;
	
	HelloRouteObjPrxHelper helper ;
	
	public void init() {
		comm = new Communicator();
		comm.setLocator("taf.tafregistry.QueryObj@tcp -h 172.27.205.110 -p 17890 -t 10000:tcp -h 172.27.205.110 -p 17990 -t 10000");
		//comm.setStat("taf.tafstat.StatObj");
		//comm = CommunicatorFactory.getComm("client.config.conf");
		helper = comm.stringToProxy("Test.HelloRouteServer.HelloRouteObj", HelloRouteObjPrxHelper.class);
	}
	
	public void test( int count , int sleepTime ) {
		for ( int i = 0 ; i < 1000; i++ ) {
			try {
				JceStringHolder out = new JceStringHolder("");;
				helper.testHello("in-"+i, out);
				System.out.println("out is "+out.getValue());
			} catch (Exception e) {
				e.printStackTrace();
			}
			try {
				Thread.sleep(sleepTime);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Main m = new Main();
		m.init();
		int count = 1000;
		int sleepTime = 1000;
		if ( args.length > 0 ) {
			count = Integer.parseInt(args[0]);
		}
		if ( args.length > 1 ) {
			sleepTime = Integer.parseInt(args[1]);
		}
		m.test(count,sleepTime);
	}

}
