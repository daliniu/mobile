package com.qq.taf.proxy.test;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.jutil.crypto.HexUtil;
import com.qq.taf.holder.JceArrayListHolder;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxy;
import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.proxy.conn.LoggerFilter;

public class TinnyTest {

	public static void testLocalType() {
		JceOutputStream out = new JceOutputStream(0);
		out.write(1, 0);
		byte[] sBufferBytes = JceUtil.getJceBufArray(out.getByteBuffer());
		JceInputStream _is = new JceInputStream(sBufferBytes);
		Integer r = -1;
		Integer t = (Integer) _is.read((Integer) r, 0, true);
		System.out.println(r);
	}

	public static void testParseList() {
		byte[] endPointBytes = HexUtil.hexStr2Bytes("0900010A060A31302E312E33362E333911574920033C0B");
		JceInputStream in = new JceInputStream(endPointBytes);
		List list = in.readList(0, false);
		System.out.println(list.size());
		for (Object o : list) {
			System.out.println(o);
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		String path = "http://q2.3g.qq.com:80/g/s";
		
		int semicolon = path.indexOf("//");
		if ( semicolon > 0  ) {
			int s2 = path.indexOf("/", semicolon+2);
			if ( s2 > 0 && s2 < path.length() ) {
				path = path.substring(s2);
			}
		}
		
		
		ByteBuffer buffer = ByteBuffer.allocate(8);
		buffer.putDouble(1.23);
		byte[] sBufferBytes = JceUtil.getJceBufArray(buffer);
		String sBuffer = HexUtil.bytes2HexStr(sBufferBytes);
		System.out.println(JceUtil.getHexdump(sBufferBytes)+">>"+sBuffer);
		
		System.out.println(HexUtil.bytes2HexStr("testVectorDouble".getBytes()));
		
		ByteBuffer buffer2 = ByteBuffer.allocate(8);
		buffer2.putDouble(4.56);
		sBufferBytes = JceUtil.getJceBufArray(buffer2);
		sBuffer = HexUtil.bytes2HexStr(sBufferBytes);
		System.out.println(JceUtil.getHexdump(sBufferBytes)+">>"+sBuffer);
		
		System.out.println(HexUtil.bytes2HexStr("testVectorDouble".getBytes()));
		
		
		
		
		// testLocalType();
		// testParseList();
		// EndpointF endPoint = new EndpointF("10.1.36.40", 22347, 3, 0);
		// JceOutputStream out = new JceOutputStream(0);
		// endPoint.writeTo(out);
		// byte[] sBufferBytes = JceUtil.getJceBufArray(out.getByteBuffer());
		// String sBuffer = HexUtil.bytes2HexStr(sBufferBytes);
		// System.out.println(JceUtil.getHexdump(sBufferBytes)+">>"+sBuffer);

		// EndpointF endPoint = new EndpointF("10.1.36.39", 22347, 3, 0);
		// JceOutputStream out = new JceOutputStream(0);
		// endPoint.writeTo(out);
		// byte[] sBufferBytes = JceUtil.getJceBufArray(out.getByteBuffer());
		// String sBuffer = HexUtil.bytes2HexStr(sBufferBytes);
		// System.out.println(JceUtil.getHexdump(sBufferBytes)+">>"+sBuffer);
		//		
		// byte[] endPointBytes =
		// HexUtil.hexStr2Bytes("0900010A060A31302E312E33362E333911574920033C0B");
		// JceInputStream in = new JceInputStream(endPointBytes);
//		LoggerFliter.addCareServer("tcp", "127.0.0.1", 23333);
//		List<Integer> defaultTimeStatInterv = new ArrayList<Integer>();
//		defaultTimeStatInterv.add(0);
//		defaultTimeStatInterv.add(5);
//		defaultTimeStatInterv.add(10);
//		defaultTimeStatInterv.add(50);
//		defaultTimeStatInterv.add(100);
//		defaultTimeStatInterv.add(200);
//		defaultTimeStatInterv.add(500);
//		defaultTimeStatInterv.add(1000);
//		defaultTimeStatInterv.add(2000);
//		defaultTimeStatInterv.add(3000);
//		
//		int _count = 0 ;
//		if ( args.length > 0  ) {
//			_count = Integer.parseInt(args[0]) ;
//		}
//		int size = 1 ;
//		if ( args.length > 1 ) {
//			size = Integer.parseInt(args[1]) ;
//		}
//		
//		Map<StatMsgHead, StatMsgBody> statmsg = new HashMap<StatMsgHead,StatMsgBody>();
//		for ( int i = 0 ; i < size ; i++ ) {
//			String masterName = "Java";
//			String slaveName = "taf.onetest.TestObj";
//			String interfaceName = "echo";
//			String masterIp = "";
//			String slaveIp = "127.0.0.1";
//			int slavePort = 39985 + (i*4);
//			int returnValue = 0;
//			StatMsgHead head = new StatMsgHead(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort,
//					returnValue);
//			int _timeoutCount = 0;
//			int _execCount = 0;
//			long _totalRspTime = 0;
//			HashMap<Integer, Integer> _intervalCount = new HashMap<Integer, Integer>();
//			_intervalCount.put(0, _count);
//			_intervalCount.put(5, 0);
//			_intervalCount.put(10, 0);
//			_intervalCount.put(50, 0);
//			_intervalCount.put(100, 0);
//			_intervalCount.put(200, 0);
//			_intervalCount.put(500, 0);
//			_intervalCount.put(1000, 0);
//			_intervalCount.put(2000, 0);
//			_intervalCount.put(3000, 0);
//			int _maxRspTime = 0;
//			int _minRspTime = 0;
//			
//			StatMsgBody body = new StatMsgBody(_count, _timeoutCount, _execCount, _totalRspTime, _intervalCount,
//					_maxRspTime, _minRspTime);
//			
//			statmsg.put(head, body);
//		}
//
//		com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
//		_os.write(statmsg, 1);
//		String _sBuffer = com.qq.jutil.crypto.HexUtil.bytes2HexStr(com.qq.taf.jce.JceUtil.getJceBufArray(_os
//				.getByteBuffer()));
//		java.util.HashMap map = new java.util.HashMap();
//		Communicator c = new Communicator();
//		JceProxy p = c.stringToProxy("taf.onestat.StatObj",JceProxy.class);
//		p.invoke("report", _sBuffer, map);
//		System.out.println(_sBuffer);
		
//		ArrayList<String> vs = new ArrayList<String>();
//		vs.add("测afd试d哦");
//		HashMap<String,String> map = new HashMap<String,String>();
//		map.put("测试key", "测试value");
//		java.util.ArrayList<java.util.HashMap<String, String>> vm = new java.util.ArrayList<java.util.HashMap<String, String>>() ;
//		HashMap<String,String> vmm = new HashMap<String,String>();
//		map.put("vmm测试key", "vmm测试value");
//		vm.add(vmm);
//		java.util.HashMap<java.util.ArrayList<String>, java.util.ArrayList<String>> mv = new java.util.HashMap<java.util.ArrayList<String>, java.util.ArrayList<String>>();
//		java.util.ArrayList<String> mvk = new java.util.ArrayList<String>();
//		mvk.add("mv测试key");
//		java.util.ArrayList<String> mvv = new java.util.ArrayList<String>();
//		mvv.add("mv测试value");
//		mv.put(mvk, mvv);
//		TestInfo testInfo = new TestInfo(true, (byte)0x01, (short)1, 2, 3L, 4.4F, 5.5, "你好", vs, map, vm, mv);
//		com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
//		testInfo.writeTo(_os);
//		String _sBuffer = com.qq.jutil.crypto.HexUtil.bytes2HexStr(com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer()));
//		
//		 byte[] _responseBytes = com.qq.jutil.crypto.HexUtil.hexStr2Bytes(_sBuffer);
//		 
//		 JceInputStream   _is = new com.qq.taf.jce.JceInputStream(_responseBytes);
//		 
//		 testInfo.readFrom(_is);

		// byte[] reads = HexUtil.hexStr2Bytes("10012C3121FF40FC5600");
		// JceMessage msg = new JceMessage(false,reads);
		// if ( msg.getResponse().iRet == 0 ) {
		// String _returnSBuffer = msg.getResponse().sBuffer;
		// byte[] _responseBytes =
		// com.qq.jutil.crypto.HexUtil.hexStr2Bytes(_returnSBuffer);
		// com.qq.taf.jce.JceInputStream _is = new
		// com.qq.taf.jce.JceInputStream(_responseBytes);
		// String _ret = "";
		// _ret = (String) _is.read(_ret, 0, true);
		// System.out.println(_ret);
		// } else {
		// System.out.println("server resp error "+ msg.getResponse().iRet);
		// }
		//		
		// List<Integer> timeStatInterv = new ArrayList<Integer>();
		// timeStatInterv.add(0);
		// timeStatInterv.add(2);
		// boolean add = false ;
		// int interv = 0 ;
		// for ( int i = 0 ; i < timeStatInterv.size() ; i++) {
		// if ( interv == timeStatInterv.get(i) ) {
		// return ;
		// }
		// if ( interv < timeStatInterv.get(i) ) {
		// timeStatInterv.add(i, interv);
		// add = true ;
		// break ;
		// }
		// }
		// if ( !add ) {
		// timeStatInterv.add(interv);
		// }
		// System.out.println(timeStatInterv);

		// byte[] endPointBytes =
		// HexUtil.hexStr2Bytes("000000120000000E10012C30014C56026869");
		// ResponsePacket response = new ResponsePacket();
		// JceInputStream _is = new JceInputStream(endPointBytes);
		// response.readFrom(_is);
		// System.out.println(response.sBuffer);
		// byte[] _responseBytes =
		// com.qq.jutil.crypto.HexUtil.hexStr2Bytes(response.sBuffer);
		// com.qq.taf.jce.JceInputStream is = new
		// com.qq.taf.jce.JceInputStream(_responseBytes);
		// String _ret = "";
		// _ret = (String) is.read(_ret, 0, true);
		// System.out.println( _ret);
		// System.out.println( HexUtil.bytes2HexStr("hi".getBytes()) );
		//		

		// try {
		// Class newoneClass = Class.forName(TestPrxHelper.class.getName());
		// Constructor cons = newoneClass.getConstructor();
		// Helper helper = (Helper)cons.newInstance();
		// } catch (Exception e) {
		// e.printStackTrace();
		// }

		// try {
		// File wf = new File("1.dat");
		// FileOutputStream os = new FileOutputStream(wf);
		// byte[] bytes =
		// HexUtil.hexStr2Bytes("720100566C000000014D740022636F6D2E71712E746865737369616E2E746573742E6265616E2E4D794F626A6563745300066D794E616D65530002703053000A6D794269727468446179640000011D616821597A7A7A");
		// os.write(bytes);
		// } catch (Exception e) {
		// e.printStackTrace();
		// }

		// System.out.println(JceUtil.getHexdump("testFunc2 out string".getBytes()));
		// ByteBuffer buffer = ByteBuffer.allocate(10);
		// buffer.putFloat((float) 1.1);
		// byte[] sBufferBytes = JceUtil.getJceBufArray(buffer);
		// System.out.println(JceUtil.getHexdump(sBufferBytes));
		//		
		// byte[] floatByte = HexUtil.hexStr2Bytes("CDCC8C3F");
		// System.out.println(ByteBuffer.wrap(floatByte).getFloat());

		// Inner inner = new Inner();
		// inner.i1 = 12345;
		// inner.s = "testinvoke";
		// JceOutputStream out = new JceOutputStream(0);
		// inner.writeTo(out);
		// byte[] sBufferBytes = JceUtil.getJceBufArray(out.getByteBuffer());
		// String sBuffer = HexUtil.bytes2HexStr(sBufferBytes);
		// System.out.println(JceUtil.getHexdump(sBufferBytes)+">>"+sBuffer);
		//		
		// JceOutputStream out2 = new JceOutputStream(0);
		// out2.write(inner, 1);
		// byte[] sBufferBytes2 = JceUtil.getJceBufArray(out2.getByteBuffer());
		// String sBuffer2 = HexUtil.bytes2HexStr(sBufferBytes2);
		// System.out.println(JceUtil.getHexdump(sBufferBytes2)+">>"+sBuffer2);

		// ByteBuffer buffer = out.getByteBuffer();
		// buffer.flip();
		// JceInputStream in = new JceInputStream(buffer);
		// inner.readFrom(in);
		// System.out.println(inner.s);
		//		
		//		
		// JceOutputStream out2 = new JceOutputStream(0);
		// out2.write(inner, 1);
		// System.out.println(out.getByteBuffer().capacity());
		//				
		// byte[] receiveBytes =
		// HexUtil.hexStr2Bytes("0000000C10012C30014C5600");
		// ResponsePacket response = new ResponsePacket();
		// JceInputStream _is = new JceInputStream(receiveBytes);
		// response.readFrom(_is);
		// System.out.println( response.iRequestId );

		// try {
		// File wf = new File("1.dat");
		// Inner wt = new Inner();
		// wt.i1 = 10;
		// wt.s = "???";
		// //write
		// JceOutputStream jos = new JceOutputStream(0);
		// wt.writeTo(jos);
		// System.out.println(HexUtil.bytes2HexStr(jos.getByteBuffer().array()));
		// FileOutputStream os = new FileOutputStream(wf);
		// os.write(jos.getByteBuffer().array(), 0,
		// jos.getByteBuffer().position());
		// System.out.println("write:" + jos.getByteBuffer().position() +
		// " bytes");
		// System.out.println(wt.toString());
		// } catch (Exception e) {
		// e.printStackTrace();
		// }

		// JceOutputStream out = new JceOutputStream(0);
		// RequestPacket request = new RequestPacket((short)1, (byte)0x00, 1001,
		// "QueryObj", "findRegistryObject","AdminRegObj", new
		// HashMap<String,String>());
		// request.writeTo(out);
		//		
		// out.write(100, 0);
		// out.write(1, 1);
		// System.out.println(HexUtil.bytes2HexStr(out.getByteBuffer().array()));

	}

}
