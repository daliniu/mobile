package com.duowan.taf.proxy.utils;

final class TimeoutNodeList<K, V> {
	TimeoutNode<K, V> head;
	TimeoutNode<K, V> tail;

	public boolean isEmpty() {
		return head == null;
	}

	public void remove(TimeoutNode<K, V> n) {
		if (n.next != null) {
			n.next.prev = n.prev;
		} else {
			tail = n.prev;
			if (tail != null)
				tail.next = null;
		}
		if (n.prev != null) {
			n.prev.next = n.next;
		} else {
			head = n.next;
			if (head != null)
				head.prev = null;
		}
	}

	public void add(TimeoutNode<K, V> n) {
		if (head == null) {
			tail = head = n;
		} else {
			n.next = head;
			head.prev = n;
			head = n;
			head.prev = null;
		}
	}

	public void moveToTop(TimeoutNode<K, V> n) {
		if (head != tail) {
			remove(n);
			add(n);
		}
	}

	public void clear() {
		head = tail = null;
	}
}
