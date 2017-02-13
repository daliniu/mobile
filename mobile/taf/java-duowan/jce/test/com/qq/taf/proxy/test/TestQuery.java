package com.qq.taf.proxy.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.qq.taf.EndpointF;
import com.qq.taf.QueryFPrx;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;
import com.qq.taf.proxy.test.Test.*;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;

public class TestQuery {

	static String objectName = "Comm.BindServer.BindObj";
	static String host1 = "127.0.0.1";
	static int port1 = 22224;
	
	static String host2 = "127.0.0.1";
	static int port2 = 22225;
	
	public static void startServer() throws IOException {
		BenchemarkService s1 = new BenchemarkService("service1");
		s1.setCoreThreadSize(1);
		s1.setMaxThreadSize(4);
		s1.setQueueSize(500);
		s1.setHost(host1);
		s1.setPort(port1);
		s1.bind();
		
		BenchemarkService s2 = new BenchemarkService("service2");
		s2.setCoreThreadSize(1);
		s2.setMaxThreadSize(4);
		s2.setQueueSize(500);
		s2.setHost(host2);
		s2.setPort(port2);
		s2.bind();
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		//query();
		echo();
//		list = queryPrx.findObjectById("TestObj");
//		System.out.println("TestObj");
//		for ( EndpointF endPoint : list  ) {
//			System.out.println(endPoint);
//		}
//		list = queryPrx.findObjectById("TestExObj");
//		System.out.println("TestExObj");
//		for ( EndpointF endPoint : list  ) {
//			System.out.println(endPoint);
//		}
	}
	
	private static void echo() {
		try {
			startServer();
			Communicator c = new Communicator();
//			ProxyConfig bindObjConfig = new ProxyConfig(objectName);
//			bindObjConfig.nodeList.add(new EndPointConfig("tcp", host1, port1, 2));
			TestBasePrx helper = c.stringToProxy(objectName+"@tcp -h "+host1+" -p "+port1+":tcp -h "+host2+" -p "+port2, TestBasePrx.class);
			for ( int i = 0 ; i < 4 ; i++) {
				String si = "si";
				JceStringHolder so = new JceStringHolder("so");
				System.out.println(helper.taf_hash(i).testStr(i, si, so));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static void query() {
		Communicator c = new Communicator();
		QueryFPrx queryPrx = c.stringToProxy("QueryObj@tcp -h 10.1.36.39 -p 17890 -t 3000", QueryFPrx.class );
		EndpointF[] list =queryPrx.findObjectById("Comm.BindServer.BindObj");
		System.out.println("Comm.BindServer.BindObj");
		for ( EndpointF endPoint : list  ) {
			System.out.println(endPoint);
		}
	}

}
