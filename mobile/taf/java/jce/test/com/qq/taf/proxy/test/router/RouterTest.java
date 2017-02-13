package com.qq.taf.proxy.test.router;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxyRouter;
import com.qq.taf.proxy.test.router.Test.HelloPrxHelper;

public class RouterTest {

	static int listenPort = 10001;
	static HelloPrxHelper proxy = null ;
	
	static class Router implements ServantProxyRouter {

		public int getGridByKey(String routerKey) {
			return 0;
		}
		
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		String objectName = "Test.HelloRouteServer.HelloRouteObj";
		if ( args.length > 0  ) {
			objectName = args[0];
		}
		
		if ( args.length > 1  ) {
			listenPort = Integer.parseInt(args[1]);
		}
		Communicator c = new Communicator("client.config.conf");
		proxy = c.stringToProxy(objectName, HelloPrxHelper.class);
		
		proxy.taf_set_router(new Router());
		listenCommand();
	}
	
	static void handlerCommand(String command,OutputStream outputStream) {
		try {
			JceStringHolder holer = new JceStringHolder();
			String s = command;
			int count = 1 ;
			try {
				count = Integer.parseInt(command);
			} catch (Exception e) {
				e.printStackTrace();
			}
			for ( int i = 0 ; i < count ; i++) {
				int ret = proxy.testHello(s, holer);
				String returnString = "ret:"+ret+" s:"+s+" holer:"+holer.getValue();
				System.out.println(returnString);
				outputStream.write((returnString+"\n").getBytes());
			}
			
			System.out.println(command);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void listenCommand() {
		ServerSocket serverSocket = null;
		try {
			serverSocket = new ServerSocket(listenPort, 1, InetAddress.getByName("localhost"));
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		while (true) {
			Socket socket = null;
			BufferedReader reader = null ;
			try {
				socket = serverSocket.accept();
				//socket.setSoTimeout(100 * 1000); 
				reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				String command = reader.readLine();
				while ( !command.equalsIgnoreCase("QUIT") ) {
					handlerCommand(command,socket.getOutputStream());
					command = reader.readLine();
				}
				try {
					socket.close();
				} catch (IOException e) {
					;
				}
				break;
			} catch (Exception e) {
				System.out.println("e:"+e);
			} finally {
				if ( null != reader ) {
					try {
						reader.close();
					} catch (IOException e) {
						;
					}
				}
			}
		}
			
		try {
			serverSocket.close();
		} catch (IOException e) {
			;
		}
	}

}
