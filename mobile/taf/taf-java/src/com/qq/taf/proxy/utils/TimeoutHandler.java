package com.qq.taf.proxy.utils;

public interface TimeoutHandler<K, V> {

	/**
	 * 淘汰的时候触发
	 * @param k
	 * @param v
	 * @param now 在map中存在的时间
	 */
	public void timeout(K k ,V v , long now);
	
}
