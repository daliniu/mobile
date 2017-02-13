package com.qq.jce.wup.test;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Hashtable;

import MiniGameProto.MSGTYPE;
import MiniGameProto.TMsgHead;
import MiniGameProto.TPackage;

import com.qq.jce.wup.ObjectCreateException;
import com.qq.jce.wup.UniPacket;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.utils.HexUtil;

public class Test {

    public static void testMap(UniPacket client) throws IOException, ObjectCreateException {
        Hashtable stringMap = new Hashtable();
        client.put("mapString", stringMap);
        Hashtable stringMapt2 = (Hashtable) client.get(stringMap,"mapString");
        System.out.println(stringMapt2.size());

        stringMap = new Hashtable();
        stringMap.put("key", "value");
        client.put("mapString", stringMap);
        stringMapt2 = (Hashtable) client.get(stringMap,"mapString");
        System.out.println(stringMapt2.get("key"));
    }

    public static void testList(UniPacket client) throws IOException, ObjectCreateException {
    	Vector stringList = new Vector();
    	stringList.addElement("中文");
        stringList.addElement("abc");
        stringList.addElement("2123");
        client.put("listString", stringList);
        System.out.println("Put List,size:" + stringList.size());
        System.out.print("List Element:");
        for (int i = 0;i < stringList.size();i++)
        {
            System.out.print(stringList.elementAt(i) + ",");
        }
        System.out.println(); 
        Vector typeVector = new Vector();
        typeVector.addElement("");
        Vector stringList2 = (Vector) client.get(typeVector,"listString");
        System.out.println("Get List,size:" + stringList2.size());
        System.out.print("Get List Element:");
        for (int i = 0;i < stringList2.size();i++)
        {
            System.out.print(stringList2.elementAt(i) + ",");
        }
        System.out.println(); 
        System.out.println(); 

        
    	
    	
//        client.put("listString", stringList);
//        Vector stringList2 = (Vector) client.get(stringList,"listString");
//        System.out.println(stringList2.size());
//
//        //stringList.add(null);
//        stringList.addElement("中文");
//        client.put("listString", stringList);
//        stringList2 = (Vector) client.get(stringList,"listString");
//        System.out.println(stringList2.elementAt(0));
    }

    public static void testArrays(UniPacket client) throws ObjectCreateException, IOException {
        byte[] arraysBytes = new byte[]{};
        client.put("arraysBytes", arraysBytes);
        byte[] bb = (byte[]) client.get(arraysBytes,"arraysBytes");
        System.out.println(bb.length);

        arraysBytes = new byte[]{1};
        client.put("arraysBytes", arraysBytes);
        byte[] bb2 = (byte[]) client.get(arraysBytes,"arraysBytes");
        System.out.println(bb2[0]);
    }

    /**
     * @param args
     */
    public static void main(String[] args) {
        try {

            String a = "0000005A1C2C3C4003560066096E4C6F67696E6E65777D00003908000206037077641800010606737472696E671D00000A06083367717174657374060375696E1800010605696E7433321D000005023FE278AC8C980CA80C";

            ByteBuffer bb = ByteBuffer.allocateDirect(4);
            bb.putInt(1).flip();
            byte[] temp = new byte[4];
            bb.get(temp);
            System.out.println(HexUtils.convert(temp));
            System.out.println(HexUtils.convert(HexUtil.intToByte(1)));
            
            
//			String digits = "1A06066E6F74696365160838303632313531392A060AB3D5D0C4CDFDCFEB3232160A68696C6C5F6D646464642207BF090D3C400150016001760B4155532D4143542D434252860B4155532D4143542D4342529600A600B600C6026974D6046C697665E600F60F00FC10F2114A8A517CFC12F11303E7F61400F61500F616000B0B";
//			byte[] bytes = HexUtils.convert(digits);
//			com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(bytes);
//			NoticeStruct struct = new NoticeStruct();
//			struct = (NoticeStruct) _is.read(struct, 1, true);
//			System.out.println(struct.sQQNo);

            UniPacket client = new UniPacket();
            client.setRequestId(1);
            client.setServantName("qqchat");
            client.setFuncName("nLoginnew");
            
//            client.put("uin", new Integer(1071806636));
//            client.put("pwd", "3gqqtest");
            byte[] vEncodedData = client.encode();
            System.out.println(HexUtils.convert(vEncodedData));
//
//            byte[] bytes = HexUtils.convert(a);
//
//            UniPacket client2 = new UniPacket();
//            client2.decode(bytes);
//            System.out.println(client2.getRequestId());
//            Enumeration keys = client2.getKeySet();
//            while ( keys.hasMoreElements() ) {
//            	  System.out.println((String)keys.nextElement());
//            }
//
//            client2 = new UniPacket();
//            client2.decode(vEncodedData);
//            System.out.println(client2.getRequestId());
//            System.out.println(client2.get("uin"));
//            keys = client2.getKeySet();
//            while ( keys.hasMoreElements() ) {
//            	  System.out.println((String)keys.nextElement());
//            }
          

//			client.setEncodeName("UTF-8");
////			
            TPackage nt = new TPackage();
			nt.iVersion = 100; // 通讯版本号
			nt.iUin = 10000; // qq号
			nt.iFlag = 0; // iFlag?
			TMsgHead mh = new TMsgHead();
			mh.nMsgID = 10001;
			mh.nMsgType = MSGTYPE._MSGTYPE_REQUEST;
			mh.nMsgLen = 10;
			nt.vecMsgHead.addElement(mh);
			nt.vecMsgData = new byte[]{1,2,3,4,5,6,7};
			
            client.put("TPackage", nt);
			//testArrays(client);
			//testList(client);
			//testMap(client);
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
