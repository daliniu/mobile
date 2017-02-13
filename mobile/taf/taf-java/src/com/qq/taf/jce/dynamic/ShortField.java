package com.qq.taf.jce.dynamic;

public final class ShortField extends NumberField {
	
	private short data;

	ShortField(short data, int tag) {
		super(tag);
		this.data = data;
	}

	@Override
	public Number getNumber() {
		return data;
	}

	public short get() {
		return data;
	}
	
	public void set(short s) {
		data = s;
	}
}
