package com.qq.taf.jce.dynamic;

public class ByteArrayField extends JceField {
	
	private byte[] data;

	ByteArrayField(byte[] data, int tag) {
		super(tag);
		this.data = data;
	}

	public byte[] get() {
		return data;
	}
	
	public void set(byte[] bs) {
		data = bs;
	}
}
