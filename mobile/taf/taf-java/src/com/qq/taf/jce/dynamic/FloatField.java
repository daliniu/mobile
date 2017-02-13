package com.qq.taf.jce.dynamic;

public class FloatField extends NumberField {

	private float data;
	
	FloatField(float data, int tag) {
		super(tag);
		this.data = data;
	}

	@Override
	public Number getNumber() {
		return data;
	}
	
	public void set(float n) {
		data = n;
	}

	public float get() {
		return data;
	}
}
