package com.qq.taf.jce.wup.test;

import java.io.ByteArrayOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import com.qq.jce.wup.UniPacket;

public class UdpTest {

	public static void main(String[] args) {
		try {
			String host = "172.25.38.67";
			int port = 55555;
			UniPacket client = new UniPacket();
			client.setRequestId(1);
			client.setServantName("qqchat" );
			client.setFuncName( "23456" );
			client.put("uin", 1071806636);
			client.put("pwd", "3gqqtest");
			byte[] message = client.encode();
			InetAddress address = InetAddress.getByName(host);
			DatagramPacket packet = new DatagramPacket(message, message.length, address, port);
			DatagramSocket dsocket = new DatagramSocket();
			dsocket.send(packet);
			dsocket.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public static byte[] convert(String digits) {

		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		for (int i = 0; i < digits.length(); i += 2) {
			char c1 = digits.charAt(i);
			if ((i + 1) >= digits.length())
				throw new IllegalArgumentException("hexUtil.odd");
			char c2 = digits.charAt(i + 1);
			byte b = 0;
			if ((c1 >= '0') && (c1 <= '9'))
				b += ((c1 - '0') * 16);
			else if ((c1 >= 'a') && (c1 <= 'f'))
				b += ((c1 - 'a' + 10) * 16);
			else if ((c1 >= 'A') && (c1 <= 'F'))
				b += ((c1 - 'A' + 10) * 16);
			else
				throw new IllegalArgumentException("hexUtil.bad");
			if ((c2 >= '0') && (c2 <= '9'))
				b += (c2 - '0');
			else if ((c2 >= 'a') && (c2 <= 'f'))
				b += (c2 - 'a' + 10);
			else if ((c2 >= 'A') && (c2 <= 'F'))
				b += (c2 - 'A' + 10);
			else
				throw new IllegalArgumentException("hexUtil.bad");
			baos.write(b);
		}
		return (baos.toByteArray());

	}

}
