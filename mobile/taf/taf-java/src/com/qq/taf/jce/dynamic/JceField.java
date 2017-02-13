package com.qq.taf.jce.dynamic;

import com.qq.taf.jce.JceDecodeException;


public class JceField {
	private int	tag;
	
	JceField(int tag) {
		this.tag = tag;
	}
	
	public int getTag() {
		return tag;
	}

	public static JceField create(byte data, int tag) {
		return new ByteField(data, tag);
	}
	
	public static JceField create(short data, int tag) {
		return new ShortField(data, tag);
	}
	
	public static JceField create(int data, int tag) {
		return new IntField(data, tag);
	}
	
	public static JceField create(long data, int tag) {
		return new LongField(data, tag);
	}
	
	public static JceField create(float data, int tag) {
		return new FloatField(data, tag);
	}
	
	public static JceField create(double data, int tag) {
		return new DoubleField(data, tag);
	}
	
	public static JceField create(String data, int tag) {
		return new StringField(data, tag);
	}
	
	public static JceField create(byte[] data, int tag) {
		return new ByteArrayField(data, tag);
	}
	
	public static JceField createList(JceField[] data, int tag) {
		return new ListField(data, tag);
	}
	
	public static JceField createMap(JceField[] keys, JceField[] values, int tag) {
		return new MapField(keys, values, tag);
	}
	
	public static JceField createStruct(JceField[] data, int tag) {
		return new StructField(data, tag);
	}
	
	private static ZeroField[] zs;
	
	static {
		zs = new ZeroField[256];
		for (int i = 0; i < zs.length; ++i)
			zs[i] = new ZeroField(i);
	}
	
	public static JceField createZero(int tag) {
		if (tag < 0 || tag >= 255)
			throw new JceDecodeException("invalid tag: " + tag);
		return zs[tag];
	}
}
