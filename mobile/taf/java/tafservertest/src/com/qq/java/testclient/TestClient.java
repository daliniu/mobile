package com.qq.java.testclient;

import java.net.Socket;
import java.util.HashMap;

import com.qq.java.testserver.testservant.Send;
import com.qq.jce.wup.UniPacket;
import com.qq.taf.jce.HexUtil;

public class TestClient {

	public static void testOld() throws Exception{
		UniPacket req = new UniPacket();
        req.setRequestId(0);
        req.setServantName("Java.TestServer.TestServant");
        req.setFuncName("testHello");
        req.put("s", "abcdefg");
		req.put("i", 100);
		req.put("l", 100000000L);
		req.put("b", true);
		req.put("c", (byte)'a');
		
		byte[] vt = new byte[]{'c','b','a'};
		req.put("vt", vt);

		HashMap<String, String> mp = new HashMap<String,String>();
		mp.put("key1", "value1");
		mp.put("key2", "value2");
		req.put("mp", mp);
		
		Send stSend = new Send();
		stSend.a = 1;
		stSend.b = "send1";
		stSend.c = 100000000000L;
		stSend.d = true;
		req.put("send", stSend);

        byte[] buffer = req.encode();
        
        UniPacket resp2 = new UniPacket();
		resp2.decode(buffer);
		System.out.println(HexUtil.bytes2HexStr(buffer));
        Socket s = new Socket("10.130.64.220", 12735) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		System.out.println(HexUtil.bytes2HexStr(rece));
		UniPacket resp = new UniPacket();
		resp.decode(rece);
		java.util.Iterator<String> its = resp.getKeySet().iterator();
		while (its.hasNext()) {
			String key = its.next();
			System.out.println(key+":"+resp.get(key,""));
		}
	}
	
	public static void testNew() throws Exception{
		UniPacket req = new UniPacket();
		req.useVersion3();
        req.setRequestId(0);
        req.setServantName("Java.TestServer.TestServant");
        req.setFuncName("testHello");
        req.put("s", "abcdefg");
		req.put("i", 100);
		req.put("l", 100000000L);
		req.put("b", true);
		req.put("c", (byte)'a');
		
		byte[] vt = new byte[]{'c','b','a'};
		req.put("vt", vt);

		HashMap<String, String> mp = new HashMap<String,String>();
		mp.put("key1", "value1");
		mp.put("key2", "value2");
		req.put("mp", mp);

		Send stSend = new Send();
		stSend.a = 1;
		stSend.b = "send1";
		stSend.c = 100000000000L;
		stSend.d = true;
		req.put("send", stSend);

        byte[] buffer = req.encode();
        
        UniPacket resp2 = new UniPacket();
		resp2.decode(buffer);
		System.out.println(HexUtil.bytes2HexStr(buffer));
        Socket s = new Socket("10.130.64.220", 12735) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		System.out.println(HexUtil.bytes2HexStr(rece));
		UniPacket resp = new UniPacket();
		resp.decode(rece);
		java.util.Iterator<String> its = resp.getKeySet().iterator();
		while (its.hasNext()) {
			String key = its.next();
			if (key.equals("r")) {
				System.out.println(key + ":" + resp.getByClass(key, ""));
			} else {
				System.out.println(key + ":" + resp.getByClass(key, 0));
			}
			
		}
	}

	public static void main(String[] args) throws Exception {
		testOld();
//		System.out.println("================================");
//		testNew();
	}
	
}
