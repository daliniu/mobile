package com.qq.java.testcase;

import java.net.Socket;

import com.qq.java.testcase.testcaseservant.TafCaseServantPrx;
import com.qq.java.testcase.testcaseservant.TafCaseServantPrxHelper;
import com.qq.jce.wup.UniPacket;
import com.qq.taf.holder.JceIntHolder;
import com.qq.taf.holder.JceLongHolder;
import com.qq.taf.holder.JceShortHolder;
import com.qq.taf.proxy.Communicator;

public class TestTestUnsigned {
	
	private static final String JAVA_OBJNAME="Java.TestCase.TafCaseServant";
	private static final String JAVA_IP="10.130.64.220";
	private static final int JAVA_PORT=12121;
	private static final String CPP_OBJNAME="Test.TafCaseServer.TafCaseServantObj";
	private static final String CPP_IP="172.27.205.110";
	private static final int CPP_PORT=30000;
	private static final String METHOD="testUnsigned";
	
	public static void main(String[] args) throws Throwable {
		testJceJava();
		testWupOldJava();
		testWupNewJava();
		testJceCpp();
		testWupOldCpp();
		testWupNewCpp();
	}
	
	private static void testJce(String title,String objname) {
		Communicator comm=new Communicator(TestTestUnsigned.class.getResource("/client.conf").getFile());
		TafCaseServantPrx prx=comm.stringToProxy(objname, TafCaseServantPrxHelper.class);
		short ubInUin=1000;
		int usInUin=10000;
		long uiInUin=100000;
		JceShortHolder ubOutUin=new JceShortHolder();
		JceIntHolder usOutUin=new JceIntHolder();
		JceLongHolder uiOutUin=new JceLongHolder();
		prx.testUnsigned(ubInUin, usInUin, uiInUin, ubOutUin, usOutUin, uiOutUin);
		System.out.println("=== output vars ("+title+") ===");
		System.out.println(ubOutUin.value+" "+usOutUin.value+" "+uiOutUin.value);
	}
	
	private static void testJceJava() {
		testJce("testJceJava",JAVA_OBJNAME);
	}
	
	private static void testJceCpp() {
		testJce("testJceCpp",CPP_OBJNAME);
	}
	
	private static void testWupOld(String title,String objname,String ip,int port) throws Throwable {
		UniPacket req = new UniPacket();
        req.setRequestId(0);
        req.setServantName(objname);
        req.setFuncName(METHOD);
        req.put("ubInUin", (short)1000);
		req.put("usInUin", (int)10000);
		req.put("uiInUin", (long)100000);
        byte[] buffer = req.encode();
		
        Socket s = new Socket(ip, port) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		UniPacket resp = new UniPacket();
		resp.decode(rece);
		
		System.out.println("=== output vars ("+title+") ===");
		System.out.println((Short)resp.get("ubOutUin")+" "+(Integer)resp.get("usOutUin")+" "+(Long)resp.get("uiOutUin"));
	}
	
	private static void testWupOldJava() throws Throwable {
		testWupOld("testWupOldJava", JAVA_OBJNAME, JAVA_IP, JAVA_PORT);
	}
	
	private static void testWupOldCpp() throws Throwable {
		testWupOld("testWupOldCpp", CPP_OBJNAME, CPP_IP, CPP_PORT);
	}
	
	private static void testWupNew(String title,String objname,String ip,int port) throws Throwable {
		UniPacket req = new UniPacket();
		req.useVersion3();
        req.setRequestId(0);
        req.setServantName(objname);
        req.setFuncName(METHOD);
        req.put("ubInUin", (short)1000);
		req.put("usInUin", (int)10000);
		req.put("uiInUin", (long)100000);
        byte[] buffer = req.encode();
		
        Socket s = new Socket(ip, port) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		UniPacket resp = new UniPacket();
		resp.decodeVersion3(rece);
		
		System.out.println("=== output vars ("+title+") ===");
		System.out.println(resp.getByClass("ubOutUin", (short)0)+" "+resp.getByClass("usOutUin", (int)0)+" "+resp.getByClass("uiOutUin", (long)0));
	}
	
	private static void testWupNewJava() throws Throwable {
		testWupNew("testWupNewJava", JAVA_OBJNAME, JAVA_IP, JAVA_PORT);
	}
	
	private static void testWupNewCpp() throws Throwable {
		testWupNew("testWupNewCpp", CPP_OBJNAME, CPP_IP, CPP_PORT);
	}
	
}
