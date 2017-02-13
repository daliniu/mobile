package com.qq.taf.proxy.conn;

import java.util.Iterator;
import java.util.Map.Entry;

import com.qq.taf.proxy.ServantFuture;
import com.qq.taf.proxy.utils.TimeoutCache;
import com.qq.taf.proxy.utils.TimeoutHandler;

public class FutureCache {
	
	static TimeoutCache<Integer,ServantFuture> futuremap = new TimeoutCache<Integer,ServantFuture>(4086);

	public static ServantFuture remove(int seq) {
		return futuremap.remove(seq);
	}

	public static ServantFuture get(int seq) {
		return futuremap.get(seq);
	}

	public static void put(int seq, ServantFuture future, TimeoutHandler<Integer, ServantFuture> handler) {
		futuremap.put(seq, future, handler, future.getAllowWaiteTimeMill());
	}
	
	public static Iterator<Entry<Integer,ServantFuture>> getIterator() {
		return futuremap.iterator();
	}
	
}
