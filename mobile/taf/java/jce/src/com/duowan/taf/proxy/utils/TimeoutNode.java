package com.duowan.taf.proxy.utils;

import com.duowan.taf.proxy.TimerProvider;

public final class TimeoutNode<K, V> {
	K key;
	V value;
	TimeoutNode<K, V> next;
	TimeoutNode<K, V> prev;
	long createTime;
	long aliveTime;
	TimeoutHandler<K,V> handler;

	TimeoutNode(K k, V v, TimeoutHandler<K,V> handler , long createTime,long aliveTime) {
		this.key = k;
		this.value = v;
		this.createTime = createTime;
		this.aliveTime = aliveTime;
		this.handler = handler;
	}

	public K getKey() {
		return key;
	}

	public V getValue() {
		return value;
	}

	public void setValue(V value) {
		createTime = TimerProvider.getNow();
		this.value = value;
	}

	public long getCreateTime() {
		return createTime;
	}

	public void setCreateTime(long time) {
		createTime = time;
	}

	public long getAliveTime() {
		return aliveTime;
	}

	public void setAliveTime(long aliveTime) {
		this.aliveTime = aliveTime;
	}
}
