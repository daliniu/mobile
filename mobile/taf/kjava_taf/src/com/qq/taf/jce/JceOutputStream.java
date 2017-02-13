package com.qq.taf.jce;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.qq.jce.wup.WupHexUtil;
import com.qq.taf.proxy.utils.HexUtil;

public final class JceOutputStream {
	private DataOutputStream bs;
	private ByteArrayOutputStream out = new ByteArrayOutputStream();

	public JceOutputStream(int capacity, String encode) {
		// 忽略capacity
		bs = new DataOutputStream(out);
		setServerEncoding(encode);
	}
	
	public JceOutputStream(int capacity) {
		// 忽略capacity
		bs = new DataOutputStream(out);
	}

	public JceOutputStream(String encode) {
		this(128);
		setServerEncoding(encode);
	}

	public JceOutputStream() {
		this(128);
	}
	
	public DataOutputStream getByteBuffer() {
		return bs;
	}

	public byte[] getFlushBytes() throws IOException {
		bs.flush();
		byte[] b = out.toByteArray();
		//System.out.println("-------JceOutputStream:"+WupHexUtil.bytes2HexStr(b));
		return b;
	}

	public void reserve(int len) {

	}

	public void writeHead(byte type, int tag) throws IOException {
		if (tag < 15) {
			byte b = (byte) ((tag << 4) | type);
			bs.write(b);
		} else if (tag < 256) {
			byte b = (byte) ((15 << 4) | type);
			bs.write(b);
			bs.write((byte) tag);
		} else {
			throw new JceEncodeException("tag is too large: " + tag);
		}
	}

	public void write(boolean b, int tag) throws IOException {
		byte by = (byte) (b ? 0x01 : 0);
		write(by, tag);
	}

	public void write(byte b, int tag) throws IOException {
		reserve(3);
		if (b == 0) {
			writeHead(JceStruct.ZERO_TAG, tag);
		} else {
			writeHead(JceStruct.BYTE, tag);
			bs.write(b);
		}
	}

	public void write(short n, int tag) throws IOException {
		reserve(4);
		if (n >= Byte.MIN_VALUE && n <= Byte.MAX_VALUE) {
			write((byte) n, tag);
		} else {
			writeHead(JceStruct.SHORT, tag);
			bs.writeShort(n);
		}
	}

	public void write(int n, int tag) throws IOException {
		reserve(6);
		if (n >= Short.MIN_VALUE && n <= Short.MAX_VALUE) {
			write((short) n, tag);
		} else {
			writeHead(JceStruct.INT, tag);
			bs.writeInt(n);
		}
	}

	public void write(long n, int tag) throws IOException {
		reserve(10);
		if (n >= Integer.MIN_VALUE && n <= Integer.MAX_VALUE) {
			write((int) n, tag);
		} else {
			writeHead(JceStruct.LONG, tag);
			bs.writeLong(n);
		}
	}

	public void write(float n, int tag) throws IOException {
		reserve(6);
		writeHead(JceStruct.FLOAT, tag);
		bs.writeFloat(n);
	}

	public void write(double n, int tag) throws IOException {
		reserve(10);
		writeHead(JceStruct.DOUBLE, tag);
		bs.writeDouble(n);
	}

	public void writeStringByte(String s, int tag) throws IOException {
		byte[] by = HexUtil.hexStr2Bytes(s);
		reserve(10 + by.length);
		if (by.length > 255) {
			writeHead(JceStruct.STRING4, tag);
			bs.writeInt(by.length);
			bs.write(by);
		} else {
			writeHead(JceStruct.STRING1, tag);
			bs.write((byte) by.length);
			bs.write(by);
		}
	}

	public void writeByteString(String s, int tag) throws IOException {
		reserve(10 + s.length());
		byte[] by = HexUtil.hexStr2Bytes(s);
		if (by.length > 255) {
			writeHead(JceStruct.STRING4, tag);
			bs.writeInt(by.length);
			bs.write(by);
		} else {
			writeHead(JceStruct.STRING1, tag);
			bs.write((byte) by.length);
			bs.write(by);
		}
	}

	public void write(String s, int tag) throws IOException {
		byte[] by;
		try {
			by = s.getBytes(sServerEncoding);
		} catch (UnsupportedEncodingException e) {
			by = s.getBytes();
		}
		reserve(10 + by.length);
		if (by.length > 255) {
			writeHead(JceStruct.STRING4, tag);
			bs.writeInt(by.length);
			bs.write(by);
		} else {
			writeHead(JceStruct.STRING1, tag);
			bs.write((byte) by.length);
			bs.write(by);
		}
	}

	public void write(Hashtable m, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.MAP, tag);
		write(m == null ? 0 : m.size(), 0);
		if (m != null) {
			Enumeration keys = m.keys();
			while (keys.hasMoreElements()) {
				Object key = keys.nextElement();
				write(key, 0);
				Object value = m.get(key);
				write(value, 1);
			}
		}
	}

	public void write(boolean[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			boolean e = l[i];
			write(e, 0);
		}
	}

	public void write(byte[] l, int tag) throws IOException {
		reserve(8 + l.length);
		writeHead(JceStruct.SIMPLE_LIST, tag);
		writeHead(JceStruct.BYTE, 0);
		write(l.length, 0);
		bs.write(l);
	}

	public void write(short[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			short e = l[i];
			write(e, 0);
		}
	}

	public void write(int[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			int e = l[i];
			write(e, 0);
		}
	}

	public void write(long[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			long e = l[i];
			write(e, 0);
		}
	}

	public void write(float[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			float e = l[i];
			write(e, 0);
		}
	}

	public void write(double[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			double e = l[i];
			write(e, 0);
		}
	}

	public void write(Object[] l, int tag) throws IOException {
		writeArray(l, tag);
	}

	private void writeArray(Object[] l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l.length, 0);
		for (int i = 0; i < l.length; i++) {
			Object e = l[i];
			write(e, 0);
		}
	}

	public void write(Vector l, int tag) throws IOException {
		reserve(8);
		writeHead(JceStruct.LIST, tag);
		write(l == null ? 0 : l.size(), 0);
		if (l != null) {
			Enumeration enumer = l.elements();
			while (enumer.hasMoreElements()) {
				Object e = enumer.nextElement();
				write(e, 0);
			}
		}
	}

	public void write(JceStruct o, int tag) throws IOException {
		reserve(2);
		writeHead(JceStruct.STRUCT_BEGIN, tag);
		o.writeTo(this);
		reserve(2);
		writeHead(JceStruct.STRUCT_END, 0);
	}

	public void write(Byte o, int tag) throws IOException {
		write(o.byteValue(), tag);
	}

	public void write(Boolean o, int tag) throws IOException {
		write(o.booleanValue(), tag);
	}

	public void write(Short o, int tag) throws IOException {
		write(o.shortValue(), tag);
	}

	public void write(Integer o, int tag) throws IOException {
		write(o.intValue(), tag);
	}

	public void write(Long o, int tag) throws IOException {
		write(o.longValue(), tag);
	}

	public void write(Float o, int tag) throws IOException {
		write(o.floatValue(), tag);
	}

	public void write(Double o, int tag) throws IOException {
		write(o.doubleValue(), tag);
	}

	public void write(Object o, int tag) throws IOException {
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
		} else if (o instanceof Hashtable) {
			write((Hashtable) o, tag);
		} else if (o instanceof Vector) {
			write((Vector) o, tag);
		} else if (o instanceof JceStruct) {
			write((JceStruct) o, tag);
		} else if (o instanceof byte[]) {
			write((byte[]) o, tag);
		} else if (o instanceof boolean[]) {
			write((boolean[]) o, tag);
		} else if (o instanceof short[]) {
			write((short[]) o, tag);
		} else if (o instanceof int[]) {
			write((int[]) o, tag);
		} else if (o instanceof long[]) {
			write((long[]) o, tag);
		} else if (o instanceof float[]) {
			write((float[]) o, tag);
		} else if (o instanceof double[]) {
			write((double[]) o, tag);
		} else if (o.getClass().isArray()) {
			writeArray((Object[]) o, tag);
		} else {
			throw new JceEncodeException("write object error: unsupport type. " + o.getClass());
		}
	}

	protected String sServerEncoding = "GBK";

	public int setServerEncoding(String se) {
		sServerEncoding = se;
		return 0;
	}

}
