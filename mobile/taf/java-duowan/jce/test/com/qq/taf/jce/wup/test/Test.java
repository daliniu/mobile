package com.qq.taf.jce.wup.test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.qq.jce.wup.ObjectCreateException;
import com.qq.jce.wup.UniPacket;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.test.HexUtils;

public class Test {

	public static void testMap(UniPacket client) throws ObjectCreateException {
		HashMap<String,String> stringMap = new HashMap<String,String>();
		client.put("mapString", stringMap);
		HashMap<String,String> stringMapt2 = client.get("mapString");
		System.out.println(stringMapt2.size());
		
		stringMap = new HashMap<String,String>();
		stringMap.put("key", "value");
		client.put("mapString", stringMap);
		stringMapt2 = client.get("mapString");
		System.out.println(stringMapt2.get("key"));
	}
	
	public static void testList(UniPacket client) throws ObjectCreateException {
		List<String> stringList = new ArrayList<String>();
		client.put("listString", stringList);
		List<String> stringList2 = client.get("listString");
		System.out.println(stringList2.size());
		
		//stringList.add(null);
		stringList.add("中文");
		client.put("listString", stringList);
		stringList2 = client.get("listString");
		System.out.println(stringList2.get(0));
	}

	public static void testArrays(UniPacket client) throws ObjectCreateException {
		byte[] arraysBytes = new byte[] {};
		client.put("arraysBytes", arraysBytes);
		byte[] bb = client.get("arraysBytes");
		System.out.println(bb.length);
		
		arraysBytes = new byte[]{1};
		client.put("arraysBytes", arraysBytes);
		byte[] bb2 = client.get("arraysBytes");
		System.out.println(bb2[0]);
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			
			String a = "0000005A1C2C3C4003560066096E4C6F67696E6E65777D00003908000206037077641800010606737472696E671D00000A06083367717174657374060375696E1800010605696E7433321D000005023FE278AC8C980CA80C";
			
//			String digits = "1A06066E6F74696365160838303632313531392A060AB3D5D0C4CDFDCFEB3232160A68696C6C5F6D646464642207BF090D3C400150016001760B4155532D4143542D434252860B4155532D4143542D4342529600A600B600C6026974D6046C697665E600F60F00FC10F2114A8A517CFC12F11303E7F61400F61500F616000B0B";
//			byte[] bytes = HexUtils.convert(digits);
//			com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(bytes);
//			NoticeStruct struct = new NoticeStruct();
//			struct = (NoticeStruct) _is.read(struct, 1, true);
//			System.out.println(struct.sQQNo);
			
			UniPacket client = new UniPacket();
			client.setRequestId(1);
			client.setServantName("qqchat" );
			client.setFuncName( "nLoginnew" );
			client.put("uin", 1071806636);
			client.put("pwd", "3gqqtest");
			byte[] vEncodedData = client.encode();
			System.out.println(JceUtil.getHexdump(vEncodedData));
			
			byte[] bytes = HexUtils.convert(a);
			
			UniPacket client2 = new UniPacket();
			client2.decode(bytes);
			System.out.println(client2.getRequestId());
			System.out.println(client2.getKeySet());
			
			client2 = new UniPacket();
			client2.decode(vEncodedData);
			System.out.println(client2.getRequestId());
			System.out.println(client2.get("uin"));
			System.out.println(client2.getKeySet());
			
//			client.setEncodeName("UTF-8");
////			
//			testArrays(client);
//			testList(client);
//			testMap(client);
//			System.out.println("LEN:" + client.encode().length);
//			FileOutputStream out = new FileOutputStream(new File("str3.txt"));
//			out.write(client.encode());
//			out.close();
		} catch (RuntimeException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
