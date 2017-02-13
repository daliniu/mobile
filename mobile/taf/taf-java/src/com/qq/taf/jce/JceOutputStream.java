package com.qq.taf.jce;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;


public class JceOutputStream
{
	private ByteBuffer bs;
	
	public JceOutputStream(ByteBuffer bs)
	{
		this.bs = bs;
	}
	
	public JceOutputStream(int capacity)
	{
		bs = ByteBuffer.allocate(capacity);
	}
	
	public JceOutputStream()
	{
		this(128);
	}

	public ByteBuffer getByteBuffer()
	{
		return bs;
	}
	
	public byte[] toByteArray() {
		byte[] newBytes = new byte[bs.position()];
		System.arraycopy(bs.array(), 0,newBytes, 0, bs.position());
		return newBytes;
	}
	
	public void reserve(int len)
	{
		if(bs.remaining() < len){
			int n = (bs.capacity() + len) * 2;
			ByteBuffer bs2 = ByteBuffer.allocate(n);
			bs2.put(bs.array(), 0, bs.position());
			bs = bs2;
			bs2 = null;
		}
	}
	
	// 写入头信息
	public void writeHead(byte type, int tag)
	{
		if(tag < 15){
			byte b = (byte) ((tag << 4) | type);
			bs.put(b);
		}else if(tag < 256){
			byte b = (byte) ((15 << 4) | type);
			bs.put(b);
			bs.put((byte) tag);
		}else{   
			throw new JceEncodeException("tag is too large: " + tag);
		}
	}
	
	public void write(boolean b, int tag)
	{
		byte by = (byte) (b ? 0x01 : 0);
		write(by, tag);
	}
	
	public void write(byte b, int tag)
	{
		reserve(3);
		if(b == 0){
			writeHead(JceStruct.ZERO_TAG, tag);
		}else{
			writeHead(JceStruct.BYTE, tag);
			bs.put(b);
		}
	}
	
	public void write(short n, int tag)
	{
		reserve(4);
		if(n >= Byte.MIN_VALUE && n <= Byte.MAX_VALUE){
			write((byte) n, tag);
		}else{
			writeHead(JceStruct.SHORT, tag);
			bs.putShort(n);
		}
	}
	
	public void write(int n, int tag)
	{
		reserve(6);
		if(n >= Short.MIN_VALUE && n <= Short.MAX_VALUE){
			write((short) n, tag);
		}else{
			writeHead(JceStruct.INT, tag);
			bs.putInt(n);
		}
	}
	
	public void write(long n, int tag)
	{
		reserve(10);
		if(n >= Integer.MIN_VALUE && n <= Integer.MAX_VALUE){
			write((int) n, tag);
		}else{
			writeHead(JceStruct.LONG, tag);
			bs.putLong(n);
		}
	}
	
	public void write(float n, int tag)
	{
		reserve(6);
		writeHead(JceStruct.FLOAT, tag);
		bs.putFloat(n);
	}
	
	public void write(double n, int tag)
	{
		reserve(10);
		writeHead(JceStruct.DOUBLE, tag);
		bs.putDouble(n);
	}
	
	public void writeStringByte(String s, int tag)
	{
		byte[] by = HexUtil.hexStr2Bytes(s);
		reserve(10 + by.length);	// 预分配足够的缓冲区
		if(by.length > 255){
			// 长度大于255，为String4类型
			writeHead(JceStruct.STRING4, tag);
			bs.putInt(by.length);
			bs.put(by);
		}else{
			// 长度小于255，位String1类型
			writeHead(JceStruct.STRING1, tag);
			bs.put((byte) by.length);
			bs.put(by);
		}
	}
	
	public void writeByteString(String s, int tag)
	{
		reserve(10 + s.length());
		byte[] by = HexUtil.hexStr2Bytes(s);
		if(by.length > 255){
			writeHead(JceStruct.STRING4, tag);
			bs.putInt(by.length);
			bs.put(by);
		}else{
			writeHead(JceStruct.STRING1, tag);
			bs.put((byte) by.length);
			bs.put(by);
		}
	}
	
	public void write(String s, int tag)
	{
		byte[] by;
		try
		{
			by = s.getBytes(sServerEncoding);
		}
		catch (UnsupportedEncodingException e)
		{
			by = s.getBytes();
		}
		reserve(10 + by.length);
		if(by.length > 255){
			writeHead(JceStruct.STRING4, tag);
			bs.putInt(by.length);
			bs.put(by);
		}else{
			writeHead(JceStruct.STRING1, tag);
			bs.put((byte) by.length);
			bs.put(by);
		}
	}
	
	public <K, V> void write(Map<K, V> m, int tag)
	{
		reserve(8);
		writeHead(JceStruct.MAP, tag);
		write(m == null ? 0 : m.size(), 0);
		if(m != null) {
			for(Map.Entry<K, V> en : m.entrySet()){
				write(en.getKey(), 0);
				write(en.getValue(), 1);
			}
		}
	}
	
	public void write(boolean[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(boolean e : l)
			write(e,  0);
	}

	public void write(byte[] l, int tag)
	{
		reserve(8 + l.length);
		writeHead(JceStruct.SIMPLE_LIST, tag);
		writeHead(JceStruct.BYTE, 0);
		write(l.length, 0);		
		bs.put(l);
	}

	public void write(short[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(short e : l)
			write(e,  0);
	}

	public void write(int[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(int e : l)
			write(e,  0);
	}

	public void write(long[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(long e : l)
			write(e,  0);
	}

	public void write(float[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(float e : l)
			write(e,  0);
	}
	
	public void write(double[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(double e : l)
			write(e,  0);
	}
	
	public <T> void write(T[] l, int tag)
	{
		writeArray(l, tag);
	}
	
	private void writeArray(Object[] l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length,  0);
		for(Object e : l)
			write(e,  0);
	}
	
	public <T> void write(Collection<T> l, int tag)
	{
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l == null ? 0 : l.size(),  0);
		if(l != null) {
			for(T e : l)
				write(e,  0);
		}
	}
	
	public void write(JceStruct o, int tag)
	{
		reserve(2);
		writeHead(JceStruct.STRUCT_BEGIN, tag);
		o.writeTo(this);
		reserve(2);
		writeHead(JceStruct.STRUCT_END, 0);
	}
	
	public void write(Byte o, int tag)
	{
		write(o.byteValue(), tag);
	}

	public void write(Boolean o, int tag)
	{
		write(o.booleanValue(), tag);
	}

	public void write(Short o, int tag)
	{
		write(o.shortValue(), tag);
	}

	public void write(Integer o, int tag)
	{
		write(o.intValue(), tag);
	}

	public void write(Long o, int tag)
	{
		write(o.longValue(), tag);
	}

	public void write(Float o, int tag)
	{
		write(o.floatValue(), tag);
	}

	public void write(Double o, int tag)
	{
		write(o.doubleValue(), tag);
	}
	
	public void write(Object o, int tag)
	{
		if (o instanceof Byte) {
			write(((Byte) o).byteValue(), tag);
		} else if (o instanceof Boolean) {
			write(((Boolean) o).booleanValue(), tag);
		} else if (o instanceof Short) {
			write(((Short) o).shortValue(), tag);
		} else if (o instanceof Integer) {
			write(((Integer) o).intValue(), tag);
		} else if (o instanceof Long) {
			write(((Long) o).longValue(), tag);
		} else if (o instanceof Float) {
			write(((Float) o).floatValue(), tag);
		} else if (o instanceof Double) {
			write(((Double) o).doubleValue(), tag);
		} else if (o instanceof String) {
			write((String) o, tag);
		} else if (o instanceof Map) {
			write((Map<?, ?>) o, tag);
		} else if (o instanceof List) {
			write((List<?>) o, tag);
		} else if (o instanceof JceStruct) {
			write((JceStruct) o, tag);
		} else if (o instanceof byte[]) {
			write((byte[])o, tag);
		} else if (o instanceof boolean[]) {
			write((boolean[])o, tag);
		} else if (o instanceof short[]) {
			write((short[])o, tag);
		} else if (o instanceof int[]) {
			write((int[])o, tag);
		} else if (o instanceof long[]) {
			write((long[])o, tag);
		} else if (o instanceof float[]) {
			write((float[])o, tag);
		} else if (o instanceof double[]) {
			write((double[])o, tag);
		} else if (o.getClass().isArray()) {
			writeArray((Object[])o, tag);
		} else if (o instanceof Collection) {
			write((Collection<?>) o, tag);
		} else {
			throw new JceEncodeException(
					"write object error: unsupport type. "+o.getClass());
		}
	}

    protected String sServerEncoding = "UTF-8";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

	public static void main(String[] args)
	{
		JceOutputStream os = new JceOutputStream();
		long n = 0x1234567890012345L;
		os.write(n, 0);
		ByteBuffer bs = os.getByteBuffer();
		System.out.println(HexUtil.bytes2HexStr(bs.array()));
		System.out.println(Arrays.toString(os.toByteArray()));
	}
}
