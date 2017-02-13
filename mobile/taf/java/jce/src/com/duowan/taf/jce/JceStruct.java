package com.duowan.taf.jce;


public abstract class JceStruct implements java.io.Serializable
{
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
//	public static final int TYPE_TCP = 1 ;
//	public static final int TYPE_UDP = 0 ;
	
	public static final int JCE_MAX_STRING_LENGTH = 100 * 1024 * 1024;
	

	
	public abstract void writeTo(JceOutputStream os);
	public abstract void readFrom(JceInputStream is);
	public abstract void display(StringBuilder sb, int level);
	
	public JceStruct newInit() {
		return null ;
	}
	
	//将自身的数据重新初始化 供对象池使用
	public void recyle() {
		
	}
	
	/**
	 * 为wsp中能取出相关信息增设的方法
	 * @param name
	 * @return
	 */
	public boolean containField(String name) {
		return false ;
	}
	/**
	 * 为wsp中能取出相关信息增设的方法
	 * @param name
	 * @return
	 */
	public Object getFieldByName(String name) {
		return null ;
	}
	/**
	 * 为wsp中能取出相关信息增设的方法
	 * @param name
	 * @param value
	 */
	public void setFieldByName(String name, Object value) {
		
	}
	
	public byte[] toByteArray() {
		JceOutputStream os = new JceOutputStream();
		writeTo(os);
		return os.toByteArray();
	}
	
	public byte[] toByteArray(String encoding) {
		JceOutputStream os = new JceOutputStream();
		os.setServerEncoding(encoding);
		writeTo(os);
		return os.toByteArray();
	}
	
	public String toString()
	{
		StringBuilder sb = new StringBuilder();
		display(sb, 0);
		return sb.toString();
	}
}
