package com.qq.taf.jce.dynamic;

public final class ByteField extends NumberField {
	private byte	data;

	ByteField(byte b, int tag) {
		super(tag);
		data = b;
	}

	@Override
	public Number getNumber() {
		return data;
	}

	public byte get() {
		return data;
	}
	
	public void set(byte n) {
		data = n;
	}
}
