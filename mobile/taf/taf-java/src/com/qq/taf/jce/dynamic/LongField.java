package com.qq.taf.jce.dynamic;

public class LongField extends NumberField {

	private long data;
	
	LongField(long data, int tag) {
		super(tag);
		this.data = data;
	}


	@Override
	public Number getNumber() {
		return data;
	}

	public long get() {
		return data;
	}
	
	public void set(long n) {
		data = n;
	}
}
