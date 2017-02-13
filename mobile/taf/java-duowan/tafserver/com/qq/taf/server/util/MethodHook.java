package com.qq.taf.server.util;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

import com.qq.taf.server.Servant;

public class MethodHook implements InvocationHandler {

	private Object obj;
	
	/**
	 * 前置方法
	 * @param original 被代理的对象 
	 * @param method 被代理的方法
	 * @param args 方法需要的参数
	 * @return 是否允许被代理的方法执行
	 */
	protected boolean before(Object original, Method method, Object[] args) {return true;}
	
	/**
	 * 后置方法，依赖before方法，只有before通过，并且代理的方法没有出现异常，该方法才被执行
	 * @param original 被代理的对象 
	 * @param method 被代理的方法
	 * @param args 方法需要的参数
	 */
	protected void after(Object original, Method method, Object[] args) {}
	
	/**
	 * 方法执行出现异常执行
	 * @param original 被代理的对象
	 * @param method 出现异常的方法
	 * @param args 方法需要的参数
	 * @param th 出现的异常
	 */
	protected void exception(Object original, Method method, Object[] args, Throwable th) { throw new RuntimeException(th);}
	
	/**
	 * 当before不通过或者出现异常的时候使用的返回值
	 * @return
	 */
	protected Object getDefaultReturnValue(){ return null;}
	
	public final Object invoke(Object proxy, Method method, Object[] args)
			throws Throwable {
		Object result = null;
		if (before(obj, method, args)) {
			try {
				result = method.invoke(obj, args);
			} catch (Throwable t) {
				exception(proxy, method, args, t);
			}
			after(obj, method, args);
			return result;
		}
		return getDefaultReturnValue();
	}
	
	public final void bind(Servant servant) {
		if(this.obj != null) {
			throw new RuntimeException("can not bind handler to servant more than 1 time");
		}
		Class[] interfaces = servant.getClass().getInterfaces();
		if(interfaces == null || interfaces.length == 0) {
			interfaces = servant.getClass().getSuperclass().getInterfaces();
			if(interfaces == null || interfaces.length == 0) {
				throw new RuntimeException(servant +" and it's superclass do not implement any interfaces");
			}
		}
		
		Object proxy = Proxy.newProxyInstance(servant.getClass().getClassLoader(), interfaces, this);
		this.obj = servant;
		servant.setProxy(proxy);
	}

}
