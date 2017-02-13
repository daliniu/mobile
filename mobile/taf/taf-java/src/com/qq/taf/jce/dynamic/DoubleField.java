package com.qq.taf.jce.dynamic;

public class DoubleField extends NumberField {

	private double data;
	
	DoubleField(double data, int tag) {
		super(tag);
		this.data = data;
	}

	@Override
	public Number getNumber() {
		return data;
	}

	public double get() {
		return data;
	}
	
	public void set(double n) {
		data = n;
	}
}
