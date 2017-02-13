package com.qq.java.testserver;

import java.lang.reflect.Method;

import com.qq.taf.server.Application;
import com.qq.taf.server.Servant;
import com.qq.taf.server.ServerConfig;
import com.qq.taf.server.util.MethodHook;

public class TestServer extends Application {

	public static final String ENCODE = "utf-8";
	@Override
	protected void initialize() throws Throwable {
		Servant servant = addServant(TestServantImpl.class, ServerConfig.application+"."+ServerConfig.serverName+".TestServant");
		MethodHook hook = new MethodHook() {

			@Override
			protected boolean before(Object proxy, Method method, Object[] args) {
				// TODO Auto-generated method stub
				System.out.println(method.getName()+" before invoke");
				return super.before(proxy, method, args);
			}

			@Override
			protected void after(Object proxy, Method method, Object[] args) {
				System.out.println(method.getName()+" after invoke");
				super.after(proxy, method, args);
			}

			@Override
			protected void exception(Object proxy, Method method,
					Object[] args, Throwable th) {
				System.out.println(method.getName()+" exception invoke, "+th);
				th.printStackTrace();
				super.exception(proxy, method, args, th);
			}

			@Override
			protected Object getDefaultReturnValue() {
				// TODO Auto-generated method stub
				return super.getDefaultReturnValue();
			}
			
		};
		//hook.bind(servant);
	}
	
	public static void main(String[] args) {
		new TestServer().start();
//		try {
//			Class[] interfaces = TestServantImpl.class.getSuperclass().getInterfaces();
//			interfaces[0].getDeclaredMethods();
//		} catch (Throwable e) {
//			e.printStackTrace();
//		} 
	}

}
