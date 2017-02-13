package com.qq.taf.jce.dynamic;

public abstract class NumberField extends JceField {
	NumberField(int tag) {
		super(tag);
	}
	public abstract Number getNumber();

	public byte byteValue() {
		return getNumber().byteValue();
	}
	public double doubleValue() {
		return getNumber().doubleValue();
	}
	public float floatValue() {
		return getNumber().floatValue();
	}
	public int intValue() {
		return getNumber().intValue();
	}
	public long longValue() {
		return getNumber().longValue();
	}
	public short shortValue() {
		return getNumber().shortValue();
	}
}
