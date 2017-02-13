package com.qq.taf.jce;

import java.io.EOFException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.qq.taf.proxy.utils.WrapByteArrayInputStream;
import com.qq.taf.proxy.utils.WrapDataInputStream;

public final class JceInputStream {
	private WrapDataInputStream bs;

	public static class HeadData {
		public byte type;
		public int tag;
	}

	public JceInputStream(byte[] bs) {
		//System.out.println("-------JceInputStream:"+WupHexUtil.bytes2HexStr(bs));
		this.bs = new WrapDataInputStream(new WrapByteArrayInputStream(bs));
	}
	
	public JceInputStream(byte[] bs,int offset) {
		//System.out.println("-------JceInputStream:"+WupHexUtil.bytes2HexStr(bs));
		this.bs = new WrapDataInputStream(new WrapByteArrayInputStream(bs,offset,bs.length-offset));
	}
	
	public JceInputStream(byte[] bs, String encode) {
		//System.out.println("-------JceInputStream:"+WupHexUtil.bytes2HexStr(bs));
		this.bs = new WrapDataInputStream(new WrapByteArrayInputStream(bs));
		setServerEncoding(encode);
	}
	
	public JceInputStream(byte[] bs,int offset, String encode) {
		//System.out.println("-------JceInputStream:"+WupHexUtil.bytes2HexStr(bs));
		this.bs = new WrapDataInputStream(new WrapByteArrayInputStream(bs,offset,bs.length-offset));
		setServerEncoding(encode);
	}

	private int readHead(HeadData hd, WrapDataInputStream bb) throws IOException {
		byte b = bb.readByte();
		hd.type = (byte) (b & 15);
		hd.tag = ((b & (15 << 4)) >> 4);
		if (hd.tag == 15) {
			hd.tag = bb.readByte();
			return 2;
		}
		return 1;
	}

	public void readHead(HeadData hd) throws IOException {
		readHead(hd, bs);
	}
	
	// 读取头信息，但不移动缓冲区的当前偏移
	private int peakHead(HeadData hd) throws IOException {
		return readHead(hd, bs.duplicate());
	}

	private void skip(int len) throws IOException {
		bs.skip(len);
	}

	public boolean skipToTag(int tag) throws IOException {
		try {
			HeadData hd = new HeadData();
			while (true) {
				int len = peakHead(hd);
				if (tag <= hd.tag || hd.type == JceStruct.STRUCT_END)
					return tag == hd.tag;
				skip(len);
				skipField(hd.type);
			}
		} catch (JceDecodeException e) {
		} catch (EOFException e) {
		}
		return false;
	}

	public void skipToStructEnd() throws IOException {
		HeadData hd = new HeadData();
		do {
			readHead(hd);
			skipField(hd.type);
		} while (hd.type != JceStruct.STRUCT_END);
	}

	private void skipField() throws IOException {
		HeadData hd = new HeadData();
		readHead(hd);
		skipField(hd.type);
	}

	private void skipField(byte type) throws IOException {
		switch (type) {
		case JceStruct.BYTE:
			skip(1);
			break;
		case JceStruct.SHORT:
			skip(2);
			break;
		case JceStruct.INT:
			skip(4);
			break;
		case JceStruct.LONG:
			skip(8);
			break;
		case JceStruct.FLOAT:
			skip(4);
			break;
		case JceStruct.DOUBLE:
			skip(8);
			break;
		case JceStruct.STRING1: {
			int len = bs.readByte();
			if (len < 0)
				len += 256;
			skip(len);
			break;
		}
		case JceStruct.STRING4: {
			skip(bs.readInt());
			break;
		}
		case JceStruct.MAP: {
			int size = read(0, 0, true);
			for (int i = 0; i < size * 2; ++i)
				skipField();
			break;
		}
		case JceStruct.LIST: {
			int size = read(0, 0, true);
			for (int i = 0; i < size; ++i)
				skipField();
			break;
		}
		case JceStruct.SIMPLE_LIST: {
			HeadData hd = new HeadData();
			readHead(hd);
			if (hd.type != JceStruct.BYTE) {
				throw new JceDecodeException("skipField with invalid type, type value: " + type + ", " + hd.type);
			}
			int size = read(0, 0, true);
			skip(size);
			break;
		}
		case JceStruct.STRUCT_BEGIN:
			skipToStructEnd();
			break;
		case JceStruct.STRUCT_END:
		case JceStruct.ZERO_TAG:
			break;
		default:
			throw new JceDecodeException("invalid type.");
		}
	}

	public boolean readBoolean(int tag, boolean isRequire) throws IOException {
		return read(false,tag,isRequire);
	}
	
	public boolean read(boolean b, int tag, boolean isRequire) throws IOException {
		byte c = read((byte) 0x0, tag, isRequire);
		return c != 0;
	}

	public byte readByte(int tag, boolean isRequire) throws IOException {
		return read((byte)'0',tag,isRequire);
	}
	
	public byte read(byte c, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				c = 0x0;
				break;
			case JceStruct.BYTE:
				c = bs.readByte();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return c;
	}

	public short readShort(int tag, boolean isRequire) throws IOException {
		return read((short)0,tag,isRequire);
	}
	
	public short read(short n, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = (short) bs.readByte();
				break;
			case JceStruct.SHORT:
				n = bs.readShort();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public int readInt(int tag, boolean isRequire) throws IOException {
		return read(0,tag,isRequire);
	}
	
	public int read(int n, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = bs.readByte();
				break;
			case JceStruct.SHORT:
				n = bs.readShort();
				break;
			case JceStruct.INT:
				n = bs.readInt();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}
	
	public long readLong(int tag, boolean isRequire) throws IOException {
		return read(0L,tag,isRequire);
	}

	public long read(long n, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = bs.readByte();
				break;
			case JceStruct.SHORT:
				n = bs.readShort();
				break;
			case JceStruct.INT:
				n = bs.readInt();
				break;
			case JceStruct.LONG:
				n = bs.readLong();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}
	
	public float readFloat(int tag, boolean isRequire) throws IOException {
		return read(0f,tag,isRequire);
	}

	public float read(float n, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.FLOAT:
				n = bs.readFloat();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public double readDouble(int tag, boolean isRequire) throws IOException {
		return read(0d,tag,isRequire);
	}
	
	public double read(double n, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.FLOAT:
				n = bs.readFloat();
				break;
			case JceStruct.DOUBLE:
				n = bs.readDouble();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public String readString(int tag, boolean isRequire) throws IOException {
		return read("",tag,isRequire);
	}

	public String read(String s, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.STRING1: {
				int len = bs.readByte();
				if (len < 0)
					len += 256;
				byte[] ss = new byte[len];
				bs.read(ss);
				try {
					s = new String(ss, sServerEncoding);
				} catch (UnsupportedEncodingException e) {
					s = new String(ss);
				}
			}
				break;
			case JceStruct.STRING4: {
				int len = bs.readInt();
				if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
					throw new JceDecodeException("String too long: " + len);
				byte[] ss = new byte[len];
				bs.read(ss);
				try {
					s = new String(ss, sServerEncoding);
				} catch (UnsupportedEncodingException e) {
					s = new String(ss);
				}
			}
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return s;
	}

	public String[] read(String[] s, int tag, boolean isRequire) throws IOException {
		return (String[]) readArray(s, tag, isRequire);
	}


	public Hashtable readMap(Hashtable m, int tag, boolean isRequire) throws IOException {
		return (Hashtable) readMap(new Hashtable(), m, tag, isRequire);
	}

	private Hashtable readMap(Hashtable mr, Hashtable m, int tag, boolean isRequire) throws IOException {
		if (m == null || m.isEmpty()) {
			// throw new
			// TafProxyDecodeException("unable to get type of key and value.");
			return new Hashtable();
		}
		// 生成代码时已经往map里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到map里面放的是什么类型的数据

		Enumeration keys = m.keys();
		Object mk = keys.nextElement();
		Object mv = m.get(mk);
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.MAP: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				for (int i = 0; i < size; ++i) {
					Object k = (Object) read(mk, 0, true);
					Object v = (Object) read(mv, 1, true);
					mr.put(k, v);
				}
			}
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return mr;
	}

	
	public boolean[] read(boolean[] l, int tag, boolean isRequire) throws IOException {
		boolean[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new boolean[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public byte[] read(byte[] l, int tag, boolean isRequire) throws IOException {
		byte[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.SIMPLE_LIST: {
				HeadData hh = new HeadData();
				readHead(hh);
				if (hh.type != JceStruct.BYTE) {
					throw new JceDecodeException("type mismatch, tag: " + tag + ", type: " + h.type + ", " + hh.type);
				}
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("invalid size, tag: " + tag + ", type: " + h.type + ", " + hh.type
							+ ", size: " + size);
				lr = new byte[size];
				bs.read(lr);
				break;
			}
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new byte[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public short[] read(short[] l, int tag, boolean isRequire) throws IOException {
		short[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new short[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public int[] read(int[] l, int tag, boolean isRequire) throws IOException {
		int[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new int[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public long[] read(long[] l, int tag, boolean isRequire) throws IOException {
		long[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new long[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public float[] read(float[] l, int tag, boolean isRequire) throws IOException {
		float[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new float[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public double[] read(double[] l, int tag, boolean isRequire) throws IOException {
		double[] lr = null;
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				lr = new double[size];
				for (int i = 0; i < size; ++i)
					lr[i] = read(lr[0], 0, true);
				break;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}

	public Object[] readArray(Object[] l, int tag, boolean isRequire) throws IOException {
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		if (l == null || l.length == 0)
			throw new JceDecodeException("unable to get type of key and value.");
		return readArrayImpl(l[0], tag, isRequire);
	}

	public Vector readArray(Vector l, int tag, boolean isRequire) throws IOException {
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		if (l == null || l.isEmpty()) {
			return new Vector();
			// throw new
			// TafProxyDecodeException("unable to get type of key and value.");
		}
		Object[] tt = readArrayImpl(l.elementAt(0), tag, isRequire);
		if(tt==null) return null;
		Vector ll = new Vector();
		for (int i = 0; i < tt.length; ++i)
			ll.addElement(tt[i]);
		return ll;
	}

	private Object[] readArrayImpl(Object mt, int tag, boolean isRequire) throws IOException {
		if (skipToTag(tag)) {
			HeadData h = new HeadData();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				
				Object[] lr = new Object[size];
				for (int i = 0; i < size; ++i) {
					Object t = (Object) read(mt, 0, true);
					lr[i] = t;
				}
				return lr;
			}
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return null;
	}

	public JceStruct read(JceStruct o, int tag, boolean isRequire) throws IOException {
		// o必须有一个无参的构造函数
		JceStruct ref = null;
		if (skipToTag(tag)) {
			try {
				ref = (JceStruct) o.getClass().newInstance();
			} catch (Exception e) {
				throw new JceDecodeException(e.getMessage());
			}

			HeadData h = new HeadData();
			readHead(h);
			if (h.type != JceStruct.STRUCT_BEGIN)
				throw new JceDecodeException("type mismatch.");
			ref.readFrom(this);
			skipToStructEnd();
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return ref;
	}

	public Object read(Object o, int tag, boolean isRequire) throws IOException {
		if (o instanceof Byte) {
			return new Byte(read((byte) 0x0, tag, isRequire));
		} else if (o instanceof Boolean) {
			return new Boolean(read(false, tag, isRequire));
		} else if (o instanceof Short) {
			return new Short(read((short) 0, tag, isRequire));
		} else if (o instanceof Integer) {
			int i = read((int) 0, tag, isRequire);
			return new Integer(i);
		} else if (o instanceof Long) {
			return new Long(read((long) 0, tag, isRequire));
		} 
//		else if (o instanceof Float) {
//			return new Float(read((float) 0, tag, isRequire));
//		} else if (o instanceof Double) {
//			return new Double(read((double) 0, tag, isRequire));
//		}
		else if (o instanceof String) {
			return String.valueOf(read("", tag, isRequire));
		} else if (o instanceof Hashtable) {
			return readMap((Hashtable) o, tag, isRequire);
		} else if (o instanceof Vector) {
			return readArray((Vector) o, tag, isRequire);
		} else if (o instanceof JceStruct) {
			return read((JceStruct) o, tag, isRequire);
		} else if (o.getClass().isArray()) {
			if (o instanceof byte[] || o instanceof Byte[]) {
				return read((byte[]) null, tag, isRequire);
			} else if (o instanceof boolean[]) {
				return read((boolean[]) null, tag, isRequire);
			} else if (o instanceof short[]) {
				return read((short[]) null, tag, isRequire);
			} else if (o instanceof int[]) {
				return read((int[]) null, tag, isRequire);
			} else if (o instanceof long[]) {
				return read((long[]) null, tag, isRequire);
			} else if (o instanceof float[]) {
				return read((float[]) null, tag, isRequire);
			} else if (o instanceof double[]) {
				return read((double[]) null, tag, isRequire);
			} else {
				return readArray((Object[]) o, tag, isRequire);
			}
		} else {
			throw new JceDecodeException("read object error: unsupport type.");
		}
	}

	protected String sServerEncoding = "GBK";

	public int setServerEncoding(String se) {
		sServerEncoding = se;
		return 0;
	}

	public static void main(String[] args) {

	}
}
