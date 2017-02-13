package com.duowan.taf.holder;

public class JceBeanHolder<T> {
	T value;

	public JceBeanHolder() {
	}

	public JceBeanHolder(T value) {
		this.value = value;
	}

	public T getValue() {
		return value;
	}
}
