package com.qq.taf.jce.dynamic;

public class IntField extends NumberField {
	private int		data;
	
	IntField(int data, int tag) {
		super(tag);
		this.data = data;
	}
	
	public int get() {
		return data;
	}
	
	public void set(int n) {
		data = n;
	}

	@Override
	public Number getNumber() {
		return data;
	}
}
