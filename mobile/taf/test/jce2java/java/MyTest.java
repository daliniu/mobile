import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;

import com.qq.one.jce.EndpointF;
import com.qq.one.jce.JceInputStream;
import com.qq.one.jce.JceOutputStream;
import com.qq.one.jce.QueryFPrxHelper;
import com.qq.one.proxy.Communicator;


public class MyTest
{
	public static void test1()
	{
		try
		{
			com.qq.Test.Inner wt = new com.qq.Test.Inner();
			wt.i1 = 10;
			wt.s = "hello";

			com.qq.Test.Inner rt = new com.qq.Test.Inner();

			//write
			JceOutputStream jos = new JceOutputStream(0);
			wt.writeTo(jos);

			File wf = new File("1.dat");
			FileOutputStream os = new FileOutputStream(wf);
			os.write(jos.getByteBuffer().array(), 0, jos.getByteBuffer().position());

			System.out.println("write:" + jos.getByteBuffer().position() + " bytes");
			System.out.println(wt.toString());


			//read
			File rf = new File("1.dat");
			FileInputStream is = new FileInputStream(rf);
			byte[] bs = new byte[(int) rf.length()];
			int len = is.read(bs);
			System.out.println("read:" + len + " bytes");
			JceInputStream jis = new JceInputStream(bs);
			rt.readFrom(jis);

			System.out.println(wt.toString());
		}
		catch (java.io.FileNotFoundException ex)
		{
			System.out.println(ex);
		}
		catch (java.io.IOException ex)
		{
			System.out.println(ex);
		}
	}
	
	public static void test_registry()
	{
		Communicator c = new Communicator();
		QueryFPrxHelper regPrx = c.stringToProxy("RegistryObj@tcp -h 10.1.36.39 -p 17890 -t 3000", QueryFPrxHelper.class);
		EndpointF[] adminEndpoint;
		adminEndpoint = regPrx.findObjectById("AdminObj");
		System.out.println(adminEndpoint[0].toString());
	}
	
	public static void main(String[] args)
	{
		//test1();
		test_registry();
	}

}


