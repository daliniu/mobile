package com.qq.taf.jce.dynamic;

public final class ZeroField extends NumberField {
	ZeroField(int tag) {
		super(tag);
	}

	@Override
	public Number getNumber() {
		return 0;
	}
	public byte byteValue() {
		return 0;
	}
	public double doubleValue() {
		return 0;
	}
	public float floatValue() {
		return 0;
	}
	public int intValue() {
		return 0;
	}
	public long longValue() {
		return 0;
	}
	public short shortValue() {
		return 0;
	}
}
