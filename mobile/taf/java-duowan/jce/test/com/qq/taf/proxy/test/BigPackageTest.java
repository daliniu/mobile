package com.qq.taf.proxy.test;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;

public class BigPackageTest {

	static StringBuffer sb = new StringBuffer();
	
	static {
		for ( int i = 0 ; i < 100000 ; i++ ) {
			sb.append("1234567890");
		}
	}
	static byte[] bb = sb.toString().getBytes();
	static int len = bb.length;
	static int port = 9000;
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if ( args.length > 0 ) {
			port = Integer.parseInt(args[0]);
		}
		try {
			ServerSocket ss = new ServerSocket(port);
			while ( true ) {
				try {
					ByteBuffer iobuffer = ByteBuffer.allocate(len+4);
					Socket s = ss.accept();
					iobuffer.putInt(len+4).put(bb).flip();
					byte[] b = new byte[len+4];
					iobuffer.get(b);
					for ( int i = 0 ; i < 10 ; i++ ) {
						System.out.println(b.length+" headLen:"+(len+4));
						s.getOutputStream().write(b);
						s.getOutputStream().flush();
					}
				} catch(Exception ex) {
					ex.printStackTrace();
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

}
