package com.qq.taf.jce;

import java.io.IOException;

public abstract class JceStruct  {
	public static final byte BYTE = 0;
	public static final byte SHORT = 1;
	public static final byte INT = 2;
	public static final byte LONG = 3;
	public static final byte FLOAT = 4;
	public static final byte DOUBLE = 5;
	public static final byte STRING1 = 6;
	public static final byte STRING4 = 7;
	public static final byte MAP = 8;
	public static final byte LIST = 9;
	public static final byte STRUCT_BEGIN = 10;
	public static final byte STRUCT_END = 11;
	public static final byte ZERO_TAG = 12;
	public static final byte SIMPLE_LIST = 13;
	//	
	// public static final int TYPE_TCP = 1 ;
	// public static final int TYPE_UDP = 0 ;

	public static final int JCE_MAX_STRING_LENGTH = 100 * 1024 * 1024;

	public String className() {
		return "com.qq.taf.jce.JceStruct";
	}
	
	public abstract void writeTo(JceOutputStream os) throws IOException;

	public abstract void readFrom(JceInputStream is) throws IOException;

}
