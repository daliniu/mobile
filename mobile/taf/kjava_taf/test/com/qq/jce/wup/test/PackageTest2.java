package com.qq.jce.wup.test;

import java.io.IOException;

import MiniGameProto.MSGTYPE;
import MiniGameProto.TMsgHead;
import MiniGameProto.TPackage;

import com.qq.jce.wup.UniPacket;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

public class PackageTest2 {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			
			String digits = "0000009d10022c3c400156017666067665726966797d00007f0800010603726571180001060a6c6973743c636861723e1d0000640d0000601872daea097841d621a2d9f58914f710250e6497dd9fcf5292ed3c82fece2cb6b310b389c05867b4819a777984aa03abb6243dc7a15ee18ca7fc82f3fbba90d61cea908e0fd39d8f8bee1e8ef24f6c1e5e2ad1d07cf5147e3cc5f49b262909e48c980ca80c";
			byte[] bytes = HexUtils.convert(digits);
			UniPacket client = new UniPacket();
			client.decode(bytes);
			client.get(new byte[]{0}, "seq");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
