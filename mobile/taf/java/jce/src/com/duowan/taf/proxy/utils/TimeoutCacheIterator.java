package com.duowan.taf.proxy.utils;

import java.util.Iterator;
import java.util.Map;

final class TimeoutCacheIterator<K, V> implements Iterator<Map.Entry<K, V>> {
	private TimeoutCache<K, V> fc;
	private TimeoutNode<K, V> next;

	private static class Entry<Key, Value> implements Map.Entry<Key, Value> {
		private Key k;
		private Value v;

		public Entry(Key k, Value v) {
			this.k = k;
			this.v = v;
		}

		public Key getKey() {
			return k;
		}

		public Value getValue() {
			return v;
		}

		public Value setValue(Value v) {
			Value o = this.v;
			this.v = v;
			return o;
		}
	}

	public TimeoutCacheIterator(TimeoutCache<K, V> fc) {
		this.fc = fc;
		fc.lock.readLock().lock();
		try {
			this.next = fc.timeoutNodeList.head;
		} finally {
			fc.lock.readLock().unlock();
		}
	}

	public boolean hasNext() {
		return next != null;
	}

	public TimeoutNode<K, V> nextNode() {
		TimeoutNode<K, V> n = next;
		fc.lock.readLock().lock();
		try {
			next = next.next;
		} finally {
			fc.lock.readLock().unlock();
		}
		return n;
	}

	public Entry<K, V> next() {
		TimeoutNode<K, V> n = nextNode();
		return new Entry<K, V>(n.key, n.value);
	}

	public void remove() {
		fc.remove(next);
	}
}
