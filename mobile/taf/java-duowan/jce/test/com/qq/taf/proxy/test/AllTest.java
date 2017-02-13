package com.qq.taf.proxy.test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import com.qq.netutil.nio.LoggerCenter;
import com.qq.taf.proxy.test.AllTest;
import com.qq.taf.proxy.test.Test.*;
import com.qq.taf.holder.JceArrayHolder;
import com.qq.taf.holder.JceArrayListHolder;
import com.qq.taf.holder.JceHashMapHolder;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.CommunicatorConfig;
import com.qq.taf.proxy.Communicator;

public class AllTest {

	static String registerProtocol = "tcp";
	static String registerHost = "127.0.0.1";
	static int registerPort = 17890;

	static Communicator c = null;
	static TestBasePrx helper = null;

	static TestExtPrx extHelper = null;
	Random r = new Random();
	BasicCallBack basicBack = new BasicCallBack();
//	ExtCallBack extBack = new ExtCallBack();

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		init(args);
		AllTest all = new AllTest();
		while ( true  ) {
			doTest(all);
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
	}

	private static void doTest(AllTest all) {
		try {
			all.testVectorStr();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testVectorInt();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testVectorDouble();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testVectorShort();
		} catch (Exception e1) {
			e1.printStackTrace();
		}
		try {
			all.testVectorFloat();
		} catch (Exception e1) {
			e1.printStackTrace();
		}
		try {
			all.testVectorByte();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testStr();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapStr();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapIntStr();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapInt();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapByte();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapIntDouble();
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			all.testMapIntFloat();
		} catch (Exception e) {
			e.printStackTrace();
		}
//		try {
//			all.testMapStruct();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		try {
//			all.testMyInfo();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		try {
//			all.testTestInfo();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		try {
//			all.testVectorStruct();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
	}
	
	class ExtCallBack extends TestExtPrxCallback {

		@Override
		public void callback_testMapStruct_exception(int ret) {
			System.out.println("callback_testMapStruct_exception "+ret);
			LoggerCenter.info("callback_testMapStruct_exception "+ret);
		}

		@Override
		public void callback_testMyInfo(int _ret, MyInfo mo) {
			System.out.println("callback_testMyInfo "+mo);
		}

		@Override
		public void callback_testMyInfo_exception(int ret) {
			System.out.println("callback_testMyInfo_exception "+ret);
			LoggerCenter.info("callback_testMyInfo_exception "+ret);
		}

		@Override
		public void callback_testTestInfo(int _ret, TestInfo to) {
			System.out.println("callback_testTestInfo "+to);
		}

		@Override
		public void callback_testTestInfo_exception(int ret) {
			System.out.println("callback_testTestInfo_exception "+ret);
			LoggerCenter.info("callback_testTestInfo_exception "+ret);
		}

		@Override
		public void callback_testVectorStruct_exception(int ret) {
			System.out.println("callback_testVectorStruct_exception "+ret);
			LoggerCenter.info("callback_testVectorStruct_exception "+ret);
		}

		@Override
		public void callback_testVectorStruct(int _ret, TestInfo[] vo) {
			System.out.println("callback_testVectorStruct "+vo);
			
		}

		@Override
		public void callback_testMapStruct(int _ret, Map<Integer, MyInfo> mo) {
			System.out.println("callback_testMapStruct "+mo);
			
		}
		
	}
	
	class BasicCallBack extends TestBasePrxCallback {

		@Override
		public void callback_testMapByte(int _ret, Map<Byte, String> mo) {
			System.out.println("callback_testMapByte "+mo);
		}

		@Override
		public void callback_testMapByte_exception(int ret) {
			System.out.println("callback_testMapByte_exception "+ret);
			LoggerCenter.info("callback_testMapByte_exception "+ret);
		}

		@Override
		public void callback_testMapInt(int _ret, Map<Integer, Integer> mo) {
			System.out.println("callback_testMapInt "+mo);
		}

		@Override
		public void callback_testMapIntDouble(int _ret, Map<Integer, Double> mo) {
			System.out.println("callback_testMapIntDouble "+mo);
		}

		@Override
		public void callback_testMapIntDouble_exception(int ret) {
			System.out.println("callback_testMapIntDouble_exception "+ret);
			LoggerCenter.info("callback_testMapIntDouble_exception "+ret);
		}

		@Override
		public void callback_testMapIntFloat(int _ret, Map<Integer, Float> mo) {
			System.out.println("callback_testMapIntFloat "+mo);
		}

		@Override
		public void callback_testMapIntFloat_exception(int ret) {
			System.out.println("callback_testMapIntFloat_exception "+ret);
			LoggerCenter.info("callback_testMapIntFloat_exception "+ret);
		}

		@Override
		public void callback_testMapIntStr(int _ret, Map<Integer, String> mo) {
			System.out.println("callback_testMapIntStr "+mo);
		}

		@Override
		public void callback_testMapIntStr_exception(int ret) {
			System.out.println("callback_testMapIntStr_exception "+ret);
			LoggerCenter.info("callback_testMapIntStr_exception "+ret);
		}

		@Override
		public void callback_testMapInt_exception(int ret) {
			System.out.println("callback_testMapInt_exception "+ret);
			LoggerCenter.info("callback_testMapInt_exception "+ret);
		}

		@Override
		public void callback_testMapStr(int _ret, Map<String, String> mo) {
			System.out.println("callback_testMapStr "+mo);
		}

		@Override
		public void callback_testMapStr_exception(int ret) {
			System.out.println("callback_testMapStr_exception "+ret);
			LoggerCenter.info("callback_testMapStr_exception "+ret);
		}

		@Override
		public void callback_testStr(int _ret, String so) {
			System.out.println("callback_testStr "+so);
		}

		@Override
		public void callback_testStr_exception(int ret) {
			System.out.println("callback_testStr_exception "+ret);
			LoggerCenter.info("callback_testStr_exception "+ret);
		}


		@Override
		public void callback_testVectorByte_exception(int ret) {
			System.out.println("callback_testVectorByte_exception "+ret);
			LoggerCenter.info("callback_testVectorByte_exception "+ret);
		}


		@Override
		public void callback_testVectorDouble_exception(int ret) {
			System.out.println("callback_testVectorDouble_exception "+ret);
			LoggerCenter.info("callback_testVectorDouble_exception "+ret);
		}


		@Override
		public void callback_testVectorFloat_exception(int ret) {
			System.out.println("callback_testVectorFloat_exception "+ret);
			LoggerCenter.info("callback_testVectorFloat_exception "+ret);
		}


		@Override
		public void callback_testVectorInt_exception(int ret) {
			System.out.println("callback_testVectorInt_exception "+ret);
			LoggerCenter.info("callback_testVectorInt_exception "+ret);
		}


		@Override
		public void callback_testVectorShort_exception(int ret) {
			System.out.println("callback_testVectorShort_exception "+ret);
			LoggerCenter.info("callback_testVectorShort_exception "+ret);
		}


		@Override
		public void callback_testVectorStr_exception(int ret) {
			System.out.println("callback_testVectorStr_exception "+ret);
			LoggerCenter.info("callback_testVectorStr_exception "+ret);
		}

		@Override
		public void callback_testVectorByte(int _ret, byte[] vo) {
			System.out.println("callback_testVectorByte "+vo);			
		}

		@Override
		public void callback_testVectorDouble(int _ret, double[] vo) {
			System.out.println("callback_testVectorDouble "+vo);			
		}

		@Override
		public void callback_testVectorFloat(int _ret, float[] vo) {
			System.out.println("callback_testVectorFloat "+vo);
		}

		@Override
		public void callback_testVectorInt(int _ret, int[] vo) {
			System.out.println("callback_testVectorInt "+vo);
		}

		@Override
		public void callback_testVectorShort(int _ret, short[] vo) {
			System.out.println("callback_testVectorShort "+vo);
		}

		@Override
		public void callback_testVectorStr(int _ret, String[] vo) {
			System.out.println("callback_testVectorStr "+vo);
		}

	}

	public static void init(String[] args) {
		String configString = "Test.TestServer.TestBaseObj@tcp -h 127.0.0.1 -p 22322 -t 10000";
		c = new Communicator();
		//c.setProperty(ClientConfig.queryEndpointsKey, registerProtocol + " -h " + registerHost + " -p " + registerPort);
		// c.setProperty(ClientConfig.reportNameKey, "taf.onestat.StatObj");
		c.setMaxInvokeTimeout(5000);
		// c.setProperty(ClientConfig.modulenameKey, "taf.onetest");
		helper = c.stringToProxy(configString, TestBasePrx.class);
		helper.taf_async_timeout(6000);

//		extHelper = c.stringToProxy("Test.TestServer.TestExtObj", TestExtPrxHelper.class);
//		extHelper.setProperty(ClientConfig.timeoutKey, String.valueOf(6000));
	}

	public void testVectorStr() {
		System.out.println("testVectorStr>>>>>>>>");
		int id = 1;
		String[] vi = new String[3];
		vi[0] = "vi";
		vi[1] = "vi123";
		vi[2] = "vi456";
		String[] tvo = new String[1];
		tvo[0] = "tvo";
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorStr(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorStr(basicBack, id, vi);
		}
	}

	public void testVectorInt() {
		System.out.println("testVectorInt>>>>>>>>");
		int id = 1;
		int[] vi = new int[3];
		vi[0] = 1;
		vi[1] = 2;
		vi[2] = 3;
		int[] tvo = new int[1];
		tvo[0] = 2;
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorInt(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorInt(basicBack, id, vi);
		}
	}

	public void testVectorDouble() {
		System.out.println("testVectorDouble>>>>>>>>");
		int id = 1;
		double[] vi = new double[3];
		vi[0] = 1.23;
		vi[1] = 2.54;
		vi[2] = 3.86;
		double[] tvo = new double[1];
		tvo[0] = 2.89;
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorDouble(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorDouble(basicBack, id, vi);
		}
	}

	public void testVectorShort() {
		System.out.println("testVectorShort>>>>>>>>");
		int id = 1;
		short[] vi = new short[3];
		vi[0] = (short) 123;
		vi[1] = (short) 456;
		vi[2] = (short) 789;
		short[] tvo = new short[1];
		tvo[0] = (short) 789;
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorShort(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorShort(basicBack, id, vi);
		}
	}

	public void testVectorFloat() {
		System.out.println("testVectorFloat>>>>>>>>");
		int id = 1;
		float[] vi = new float[3];
		vi[0] = (float) 123;
		vi[1] = (float) 456;
		vi[2] = (float) 789;
		float[] tvo = new float[1];
		tvo[0] = (short) 789;
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorFloat(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorFloat(basicBack, id, vi);
		}
	}

	public void testVectorByte() {
		System.out.println("testVectorByte>>>>>>>>");
		int id = 1;
		byte[] vi = new byte[3];
		vi[0] = (byte)1;
		vi[1] = (byte)2;
		vi[2] = (byte)3;
		byte[] tvo = new byte[1];
		tvo[0] = (byte)2;
		JceArrayHolder vo = new JceArrayHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testVectorByte(id, vi, vo);
			System.out.println("returnInt:" + returnInt);
			System.out.println("vo:" + vo.getValue());
		} else {
			helper.async_testVectorByte(basicBack, id, vi);
		}
	}

	public void testStr() {
		System.out.println("testStr>>>>>>>>");
		int id = 1;
		String si = "si";
		JceStringHolder so = new JceStringHolder("so");
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testStr(id, si, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.getValue());
		} else {
			helper.async_testStr(basicBack, id, si);
		}
	}

	public void testMapStr() {
		System.out.println("testMapStr>>>>>>>>");
		int id = 1;
		HashMap<String, String> vi = new HashMap<String, String>();
		vi.put("key", "value");
		vi.put("key123", "value123");
		HashMap<String, String> tvo = new HashMap<String, String>();
		tvo.put("key2", "value2");
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapStr(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapStr(basicBack, id, vi);
		}
	}

	public void testMapIntStr() {
		System.out.println("testMapIntStr>>>>>>>>");
		int id = 1;
		HashMap<Integer, String> vi = new HashMap<Integer, String>();
		vi.put(1, "value");
		HashMap<Integer, String> tvo = new HashMap<Integer, String>();
		tvo.put(2, "value2");
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapIntStr(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapIntStr(basicBack, id, vi);
		}
	}

	public void testMapInt() {
		System.out.println("testMapInt>>>>>>>>");
		int id = 1;
		HashMap<Integer, Integer> vi = new HashMap<Integer, Integer>();
		vi.put(1, 11);
		HashMap<Integer, Integer> tvo = new HashMap<Integer, Integer>();
		tvo.put(2, 22);
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapInt(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapInt(basicBack, id, vi);
		}
	}

	public void testMapByte() {
		System.out.println("testMapByte>>>>>>>>");
		int id = 1;
		HashMap<Byte, String> vi = new HashMap<Byte, String>();
		vi.put((byte) 1, "123a");
		HashMap<Byte, String> tvo = new HashMap<Byte, String>();
		tvo.put((byte) 2, "456b");
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapByte(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapByte(basicBack, id, vi);
		}
	}

	public void testMapIntDouble() {
		System.out.println("testMapIntDouble>>>>>>>>");
		int id = 1;
		HashMap<Integer, Double> vi = new HashMap<Integer, Double>();
		vi.put(1, (double) 1.23);
		HashMap<Integer, Double> tvo = new HashMap<Integer, Double>();
		tvo.put(2, (double) 4.56);
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapIntDouble(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapIntDouble(basicBack, id, vi);
		}
	}

	public void testMapIntFloat() {
		System.out.println("testMapIntFloat>>>>>>>>");
		int id = 1;
		HashMap<Integer, Float> vi = new HashMap<Integer, Float>();
		vi.put(1, (float) 123);
		HashMap<Integer, Float> tvo = new HashMap<Integer, Float>();
		tvo.put(2, (float) 456);
		JceHashMapHolder so = new JceHashMapHolder(tvo);
		if (r.nextInt(2) == 0) {
			int returnInt = helper.testMapIntFloat(id, vi, so);
			System.out.println("returnInt:" + returnInt);
			System.out.println("so:" + so.value);
		} else {
			helper.async_testMapIntFloat(basicBack, id, vi);
		}
	}

//	public void testMapStruct() {
//		System.out.println("testMapStruct>>>>>>>>");
//		int id = 1;
//		MyInfo info = new MyInfo(123, "my123");
//		MyInfo info2 = new MyInfo(1234, "my1234");
//		MyInfo info3 = new MyInfo(12345, "my12345");
//		HashMap<Integer, MyInfo> mi = new HashMap<Integer, MyInfo>();
//		mi.put(1, info);
//		mi.put(2, info2);
//		mi.put(3, info3);
//		HashMap<Integer, MyInfo> tvo = new HashMap<Integer, MyInfo>();
//		MyInfo info4 = new MyInfo(123456, "my123456");
//		tvo.put(5, info4);
//		JceHashMapHolder so = new JceHashMapHolder(tvo);
//		if (r.nextInt(2) == 0) {
//		int returnInt = extHelper.testMapStruct(id, mi, so);
//		System.out.println("returnInt:" + returnInt);
//		System.out.println("so:" + so.value);
//		} else {
//			extHelper.async_testMapStruct(extBack, id, mi);
//		}
//	}
//
//	public void testMyInfo() {
//		System.out.println("testMyInfo>>>>>>>>");
//		int id = 1;
//		MyInfo info = new MyInfo(123, "my123");
//		MyInfo info4 = new MyInfo(123456, "my123456");
//		MyInfoHolder so = new MyInfoHolder(info4);
//		if (r.nextInt(2) == 0) {
//		int returnInt = extHelper.testMyInfo(id, info, so);
//		System.out.println("returnInt:" + returnInt);
//		System.out.println("so:" + so.value);
//		} else {
//			extHelper.async_testMyInfo(extBack, id, info);
//		}
//	}

//	public void testTestInfo() {
//		System.out.println("testTestInfo>>>>>>>>");
//		int id = 1;
//		String[] vs = new String[1];
//		vs[0] = "1111111测afd试d哦";
//		HashMap<String, String> map = new HashMap<String, String>();
//		map.put("测试key", "测试value");
//		HashMap<String, String>[] vm = new HashMap[1];
//		HashMap<String, String> vmm = new HashMap<String, String>();
//		map.put("1111111vmm测试key", "vmm测试value");
//		vm[0] = vmm;
//		java.util.HashMap<String[],String[]> mv = new java.util.HashMap<String[], String[]>();
//		String[] mvk = new String[1];
//		mvk[0] = "mv测试key";
//		String[] mvv = new String[1];
//		mvv[0] = "mv测试value";
//		mv.put(mvk, mvv);
//		MyInfo[] vt = new MyInfo[1];
//		MyInfo info = new MyInfo(123, "my123");
//		vt[0] = info;
//		TestInfo testInfo = new TestInfo(true, (byte) 0x01, (short) 1, 2, 3L, 4.4F, 5.5, "你好", vt, vs, map, vm, mv);
//
//		TestInfoHolder h = getTestInfoHolder();
//		if (r.nextInt(2) == 0) {
//		int returnInt = extHelper.testTestInfo(id, testInfo, h);
//		System.out.println("returnInt:" + returnInt);
//		System.out.println("so:" + h.value);
//		} else {
//			extHelper.async_testTestInfo(extBack, id, testInfo);
//		}
//	}

	public static TestInfoHolder getTestInfoHolder() {
		String[] vs = new String[1];
		vs[0] = "1111111测afd试d哦";
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("11111111测试key", "测试value");
		HashMap<String, String>[] vm = new HashMap[1];
		HashMap<String, String> vmm = new HashMap<String, String>();
		map.put("1111111vmm测试key", "vmm测试value");
		vm[0] = vmm;
		java.util.HashMap<String[],String[]> mv = new java.util.HashMap<String[], String[]>();
		String[] mvk = new String[1];
		mvk[0] = "mv测试key";
		String[] mvv = new String[1];
		mvv[0] = "mv测试value";
		mv.put(mvk, mvv);
		MyInfo[] vt = new MyInfo[1];
		MyInfo info = new MyInfo(123, "my123");
		vt[0] = info;
		TestInfo testInfo = new TestInfo(true, (byte) 0x01, (short) 1, 2, 3L, 4.4F, 5.5, "你好", vt, vs, map, vm, mv);
		TestInfoHolder hodler = new TestInfoHolder(testInfo);
		return hodler;
	}

//	public void testVectorStruct() {
//		System.out.println("testVectorStruct>>>>>>>>");
//		int id = 1;
//		String[] vs = new String[1];
//		vs[0] = "1111111测afd试d哦";
//		HashMap<String, String> map = new HashMap<String, String>();
//		map.put("测试key", "测试value");
//		HashMap<String, String>[] vm = new HashMap[1];
//		HashMap<String, String> vmm = new HashMap<String, String>();
//		map.put("1111111vmm测试key", "vmm测试value");
//		vm[0] = vmm;
//		java.util.HashMap<String[],String[]> mv = new java.util.HashMap<String[], String[]>();
//		String[] mvk = new String[1];
//		mvk[0] = "mv测试key";
//		String[] mvv = new String[1];
//		mvv[0] = "mv测试value";
//		mv.put(mvk, mvv);
//		MyInfo[] vt = new MyInfo[1];
//		MyInfo info = new MyInfo(123, "my123");
//		vt[0] = info;
//		TestInfo testInfo = new TestInfo(true, (byte) 0x01, (short) 1, 2, 3L, 4.4F, 5.5, "你好", vt, vs, map, vm, mv);
//		
//		TestInfo[] mi = new TestInfo[1];
//		mi[0] = testInfo;
//		JceArrayHolder so = new JceArrayHolder(getJceArrayListHolder());
//		if (r.nextInt(2) == 0) {
//		int returnInt = extHelper.testVectorStruct(id, mi, so);
//		System.out.println("returnInt:" + returnInt);
//		System.out.println("so:" + so.value);
//		} else {
//			extHelper.async_testVectorStruct(extBack, id, mi);
//		}
//	}

	public static TestInfo[] getJceArrayListHolder() {
		String[] vs = new String[1];
		vs[0] = "1111111测afd试d哦";
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("测试key", "测试value");
		HashMap<String, String>[] vm = new HashMap[1];
		HashMap<String, String> vmm = new HashMap<String, String>();
		map.put("1111111vmm测试key", "vmm测试value");
		vm[0] = vmm;
		java.util.HashMap<String[],String[]> mv = new java.util.HashMap<String[], String[]>();
		String[] mvk = new String[1];
		mvk[0] = "mv测试key";
		String[] mvv = new String[1];
		mvv[0] = "mv测试value";
		mv.put(mvk, mvv);
		MyInfo[] vt = new MyInfo[1];
		MyInfo info = new MyInfo(123, "my123");
		vt[0] = info;
		TestInfo testInfo = new TestInfo(true, (byte) 0x01, (short) 1, 2, 3L, 4.4F, 5.5, "你好", vt, vs, map, vm, mv);
		TestInfo[] tvo = new TestInfo[1];
		tvo[0] = testInfo;
		return tvo;
	}

}
