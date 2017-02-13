package com.qq.java.testcase;

import java.net.Socket;

import com.qq.java.testcase.testcaseservant.TafCaseServantPrx;
import com.qq.java.testcase.testcaseservant.TafCaseServantPrxHelper;
import com.qq.java.testcase.testcaseservant.UserInfo;
import com.qq.java.testcase.testcaseservant.UserInfoHolder;
import com.qq.jce.wup.UniPacket;
import com.qq.taf.proxy.Communicator;

public class TestGetUser {

	private static final String JAVA_OBJNAME="Java.TestCase.TafCaseServant";
	private static final String JAVA_IP="10.130.64.220";
	private static final int JAVA_PORT=12121;
	private static final String CPP_OBJNAME="Test.TafCaseServer.TafCaseServantObj";
	private static final String CPP_IP="172.27.205.110";
	private static final int CPP_PORT=30000;
	private static final String METHOD="getUser";
	
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
		UserInfo ui=new UserInfo();
		ui.ubQQ=1000;
		ui.usQQ=10000;
		ui.uiQQ=100000;
		ui.uiTest=1000000;
		UserInfoHolder uih=new UserInfoHolder();
		prx.getUser(ui, uih);
		System.out.println("=== output user ("+title+") ===");
		System.out.println(uih.value.toString());
	}
	
	private static void testJceJava() {
		testJce("testJceJava",JAVA_OBJNAME);
	}
	
	private static void testJceCpp() {
		testJce("testJceCpp",CPP_OBJNAME);
	}
	
	private static void testWupOldJava() throws Throwable {
		UniPacket req = new UniPacket();
        req.setRequestId(0);
        req.setServantName(JAVA_OBJNAME);
        req.setFuncName(METHOD);
		UserInfo ui=new UserInfo();
		ui.ubQQ=1000;
		ui.usQQ=10000;
		ui.uiQQ=100000;
		ui.uiTest=1000000;
        req.put("inUser", ui);
        byte[] buffer = req.encode();
		
        Socket s = new Socket(JAVA_IP, JAVA_PORT) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		UniPacket resp = new UniPacket();
		resp.decode(rece);
		
		System.out.println("=== output user (testWupOldJava) ===");
		UserInfo uii=(UserInfo)resp.get("outUser");
		System.out.println(uii.toString());
	}
	
	private static void testWupOldCpp() throws Throwable {
		UniPacket req = new UniPacket();
        req.setRequestId(0);
        req.setServantName(CPP_OBJNAME);
        req.setFuncName(METHOD);
		Test.UserInfo ui=new Test.UserInfo();
		ui.ubQQ=1000;
		ui.usQQ=10000;
		ui.uiQQ=100000;
		ui.uiTest=1000000;
        req.put("inUser", ui);
        byte[] buffer = req.encode();
		
        Socket s = new Socket(CPP_IP, CPP_PORT) ;
        s.getOutputStream().write(buffer);
        s.getOutputStream().flush();
        byte[] temp = new byte[2048];
        int readed = s.getInputStream().read(temp);
        byte[] rece = new byte[readed];
        System.arraycopy(temp, 0, rece, 0, readed);
		s.close();
		UniPacket resp = new UniPacket();
		resp.decode(rece);
		
		System.out.println("=== output user (testWupOldCpp) ===");
		Test.UserInfo uii=(Test.UserInfo)resp.get("outUser");
		System.out.println(uii.toString());
	}
	
	private static void testWupNew(String title,String objname,String ip,int port) throws Throwable {
		UniPacket req = new UniPacket();
		req.useVersion3();
        req.setRequestId(0);
        req.setServantName(objname);
        req.setFuncName(METHOD);
		UserInfo ui=new UserInfo();
		ui.ubQQ=1000;
		ui.usQQ=10000;
		ui.uiQQ=100000;
		ui.uiTest=1000000;
        req.put("inUser", ui);
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
		UserInfo uiType=new UserInfo();
		UserInfo uii=resp.getByClass("outUser", uiType);
		System.out.println(uii.toString());
	}
	
	private static void testWupNewJava() throws Throwable {
		testWupNew("testWupNewJava", JAVA_OBJNAME, JAVA_IP, JAVA_PORT);
	}
	
	private static void testWupNewCpp() throws Throwable {
		testWupNew("testWupNewCpp", CPP_OBJNAME, CPP_IP, CPP_PORT);
	}
	
}
