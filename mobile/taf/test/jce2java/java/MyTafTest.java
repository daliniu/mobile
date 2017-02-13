

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.qq.one.jce.EndpointF;
import com.qq.one.jce.QueryFPrx;
import com.qq.one.jce.QueryFPrxHelper;
import com.qq.one.proxy.Communicator;
import com.qq.one.proxy.ClientConfig;
import com.qq.one.proxy.conn.EndPointInfo;
import com.qq.Server1.*;
import com.qq.Server2.*;

public class MyTafTest {

	static String objectName = "one.onetest.TestObj";
	static String host1 = "172.25.38.67";
	static int port1 = 31111;
	
	static String host2 = "172.25.38.68";
	static int port2 = 31111;

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		test();
	}
	
	private static void test() {
		try{
			Communicator c = new Communicator();
			TestPrxHelper helper = c.stringToProxy(objectName+"@tcp -h "+host1+" -p "+port1, TestPrxHelper.class);
			
			com.qq.Server1.TestInfo in , out;
			in = new com.qq.Server1.TestInfo();
			
			in.vs = new String[0];
			
			in.m = new java.util.HashMap<String, String>();
			
			in.vm = new java.util.HashMap[1];
			in.vm[0] = new java.util.HashMap<String, String>();
			in.vm[0].put("a","b");

			in.vvv = new String[1][1][1];
			in.vvv[0] = new String[1][1];
			in.vvv[0][0] = new String[1];
			in.vvv[0][0][0] = "vvv--";
			
			in.vvmv = new java.util.HashMap[1][1];
			in.vvmv[0] = new java.util.HashMap[1];
			in.vvmv[0][0] = new java.util.HashMap<String, String[]>();
			in.vvmv[0][0].put("vvmv", in.vvv[0][0]);
			
			AInfo a = new AInfo();
			a.i = 1231231231;
			a.s = "test ainfo in vector中文";
			in.vst = new AInfo[1];
			in.vst[0] = a;
			
			in.vf = new float[3];
			in.vf[0]= 0.1f;
			in.vf[1]= 0.2f;
			in.vf[2]= 0.3f;
			
			out = helper.get(in);
			

			//System.out.println(out.toString());

			System.out.println(out.vvv[0][0][0]);
			System.out.println(out.vst[0].s);
			
			
			System.out.println("OK=================");
		}
		catch(Exception e) {
				e.printStackTrace();
		}

	}

}
