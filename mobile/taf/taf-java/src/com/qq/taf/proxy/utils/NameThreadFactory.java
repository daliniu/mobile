package com.qq.taf.proxy.utils;

import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * 用于创建线程池的可命名线程工厂 
 * @author fanzhang
 */
public class NameThreadFactory implements ThreadFactory {
	
	private String name;
	private AtomicInteger count;
	
	public NameThreadFactory(String name) {
		this.name=name;
		this.count=new AtomicInteger(1);
	}

	public Thread newThread(Runnable r) {
		return new Thread(r,name+"_"+count.getAndIncrement());
	}
	
}
