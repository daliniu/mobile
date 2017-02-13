package com.duowan.taf.jce;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;


/**
 * 数据读取流
 * @author meteorchen
 *
 */
public final class JceInputStream {
	private ByteBuffer bs;	// 缓冲区
	
	
	/**
	 * 头数据
	 * @author meteorchen
	 *
	 */
	public static class HeadData {
		
		public byte type;
		public int tag;
		
		public void clear() {
			type = 0 ;
			tag = 0 ;
		}
	}

	public JceInputStream() {
		
	}

	
	public JceInputStream(ByteBuffer bs) {
		this.bs = bs;
	}

	public JceInputStream(byte[] bs) {
		this.bs = ByteBuffer.wrap(bs);
	}
	
	public JceInputStream(byte[] bs, int pos) {
		this.bs = ByteBuffer.wrap(bs);
		this.bs.position(pos);
	}
	
	
	public void warp(byte[] bs) {
		if ( null != this.bs) {
			this.bs.clear();
		}
		this.bs = ByteBuffer.wrap(bs);
	}

	/**
	 * 读取数据头
	 * @param hd	读取到的头信息
	 * @param bb	缓冲
	 * @return 读取的字节数
	 */
	public static int readHead(HeadData hd, ByteBuffer bb) {
		byte b = bb.get();
		hd.type = (byte) (b & 15);
		hd.tag = ((b & (15 << 4)) >> 4);
		if (hd.tag == 15) {
			hd.tag = bb.get();
			return 2;
		}
		return 1;
	}

	public void readHead(HeadData hd) {
		readHead(hd, bs);
	}

	// 读取头信息，但不移动缓冲区的当前偏移
	private int peakHead(HeadData hd) {
		return readHead(hd, bs.duplicate());
	}

	// 跳过若干字节
	private void skip(int len) {
		bs.position(bs.position() + len);
	}

	// 跳到指定的tag的数据之前
	public boolean skipToTag(int tag) {
		try {
			HeadData hd = new HeadData();
			while (true) {
				int len = peakHead(hd);
				if (hd.type == JceStruct.STRUCT_END) {
					return false;
				}
				if (tag <= hd.tag)
					return tag == hd.tag;
				skip(len);
				skipField(hd.type);
			}
		} catch (JceDecodeException e) {
		} catch (BufferUnderflowException e) {
		}
		return false;
	}

	// 跳到当前结构的结束位置
	public void skipToStructEnd() {
		HeadData hd = new HeadData();
		do {
			readHead(hd);
			skipField(hd.type);
		} while (hd.type != JceStruct.STRUCT_END);
	}
	
	// 跳过一个字段
	private void skipField() {
		HeadData hd = new HeadData();
		readHead(hd);
		skipField(hd.type);
	}

	private void skipField(byte type) {
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
			int len = bs.get();
			if (len < 0)
				len += 256;
			skip(len);
			break;
		}
		case JceStruct.STRING4: {
			skip(bs.getInt());
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
			if(hd.type != JceStruct.BYTE){
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

	public boolean read(boolean b, int tag, boolean isRequire) {
		byte c = read((byte) 0x0, tag, isRequire);
		return c != 0;
	}

	public byte read(byte c, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				c = 0x0;
				break;
			case JceStruct.BYTE:
				c = bs.get();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return c;
	}

	public short read(short n, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = (short) bs.get();
				break;
			case JceStruct.SHORT:
				n = bs.getShort();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public int read(int n, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = bs.get();
				break;
			case JceStruct.SHORT:
				n = bs.getShort();
				break;
			case JceStruct.INT:
				n = bs.getInt();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public long read(long n, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.BYTE:
				n = bs.get();
				break;
			case JceStruct.SHORT:
				n = bs.getShort();
				break;
			case JceStruct.INT:
				n = bs.getInt();
				break;
			case JceStruct.LONG:
				n = bs.getLong();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public float read(float n, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.FLOAT:
				n = bs.getFloat();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public double read(double n, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.ZERO_TAG:
				n = 0;
				break;
			case JceStruct.FLOAT:
				n = bs.getFloat();
				break;
			case JceStruct.DOUBLE:
				n = bs.getDouble();
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return n;
	}

	public String readByteString(String s, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.STRING1: {
				int len = bs.get();
				if (len < 0)
					len += 256;
				byte[] ss = new byte[len];
				bs.get(ss);
				s = HexUtil.bytes2HexStr(ss);
			}
				break;
			case JceStruct.STRING4: {
				int len = bs.getInt();
				if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
					throw new JceDecodeException("String too long: " + len);
				byte[] ss = new byte[len];
				bs.get(ss);
				s = HexUtil.bytes2HexStr(ss);
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
	
	public String read(String s , int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.STRING1: {
				int len = bs.get();
				if (len < 0)
					len += 256;
				byte[] ss = new byte[len];
				bs.get(ss);
				try{
					s = new String(ss, sServerEncoding);
				}
				catch (UnsupportedEncodingException e){
					s = new String(ss);
				}
			}
				break;
			case JceStruct.STRING4: {
				int len = bs.getInt();
				if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
					throw new JceDecodeException("String too long: " + len);
				byte[] ss = new byte[len];
				bs.get(ss);
				try{
					s = new String(ss, sServerEncoding);
				}
				catch (UnsupportedEncodingException e){
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

	public String readString(int tag, boolean isRequire) {
		String s = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.STRING1: {
				int len = bs.get();
				if (len < 0)
					len += 256;
				byte[] ss = new byte[len];
				bs.get(ss);
				try{
					s = new String(ss, sServerEncoding);
				}
				catch (UnsupportedEncodingException e){
					s = new String(ss);
				}
			}
				break;
			case JceStruct.STRING4: {
				int len = bs.getInt();
				if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
					throw new JceDecodeException("String too long: " + len);
				byte[] ss = new byte[len];
				bs.get(ss);
				try{
					s = new String(ss, sServerEncoding);
				}
				catch (UnsupportedEncodingException e){
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
	
	public String[] read(String[] s, int tag, boolean isRequire) {
		return readArray(s, tag, isRequire);
	}

	public Map<String, String> readStringMap(int tag, boolean isRequire) {
		HashMap<String, String> mr = new HashMap<String, String>();
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.MAP: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				for (int i = 0; i < size; ++i) {
					String k = readString(0, true);
					String v = readString(1, true);
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

	public <K, V> HashMap<K, V> readMap(Map<K, V> m, int tag, boolean isRequire) {
		return (HashMap<K, V>) readMap(new HashMap<K, V>(), m, tag, isRequire);
	}

	@SuppressWarnings("unchecked")
	private <K, V> Map<K, V> readMap(Map<K, V> mr, Map<K, V> m, int tag, boolean isRequire) {
		if (m == null || m.isEmpty()) {
			//throw new TafProxyDecodeException("unable to get type of key and value.");
			return new HashMap();
		}
			

		// 生成代码时已经往map里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到map里面放的是什么类型的数据
		Iterator<Map.Entry<K, V>> it = m.entrySet().iterator();
		Map.Entry<K, V> en = it.next();
		K mk = en.getKey();
		V mv = en.getValue();

		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.MAP: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				for (int i = 0; i < size; ++i) {
					K k = (K) read(mk, 0, true);
					V v = (V) read(mv, 1, true);
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

	@SuppressWarnings("unchecked")
	public List readList(int tag, boolean isRequire) {
		List lr = new ArrayList();
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				for (int i = 0; i < size; ++i) {
					HeadData subH = new HeadData();
					readHead(subH);
					switch (subH.type) {
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
						int len = bs.get();
						if (len < 0)
							len += 256;
						skip(len);
					}
						break;
					case JceStruct.STRING4: {
						skip(bs.getInt());
					}
						break;
					case JceStruct.MAP: {

					}
						break;
					case JceStruct.LIST: {

					}
						break;
					case JceStruct.STRUCT_BEGIN:
						try {
							Class<?> newoneClass = Class.forName(JceStruct.class.getName());
							Constructor<?> cons = newoneClass.getConstructor();
							JceStruct struct = (JceStruct) cons.newInstance();
							struct.readFrom(this);
							skipToStructEnd();
							lr.add(struct);
						} catch (Exception e) {
							e.printStackTrace();
							throw new JceDecodeException("type mismatch." + e);
						}
						break;
					case JceStruct.ZERO_TAG:
						lr.add(new Integer(0));
						break;
					default:
						throw new JceDecodeException("type mismatch.");
					}
					// T t = read(mt, 0, true);
					// lr.add(t);
				}
			}
				break;
			default:
				throw new JceDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return lr;
	}
	
	public boolean[] read(boolean[] l, int tag, boolean isRequire){
		boolean[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public byte[] read(byte[] l, int tag, boolean isRequire){
		byte[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.SIMPLE_LIST: {
				HeadData hh = new HeadData();
				readHead(hh);
				if(hh.type != JceStruct.BYTE){
					throw new JceDecodeException("type mismatch, tag: " + tag + ", type: " + hd.type + ", " + hh.type);
				}
				int size = read(0, 0, true);
				if(size < 0)
					throw new JceDecodeException("invalid size, tag: " + tag + ", type: " + hd.type + ", " + hh.type + ", size: " + size);
				lr = new byte[size];
				bs.get(lr);
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
	
	public short[] read(short[] l, int tag, boolean isRequire){
		short[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public int[] read(int[] l, int tag, boolean isRequire){
		int[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public long[] read(long[] l, int tag, boolean isRequire){
		long[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public float[] read(float[] l, int tag, boolean isRequire){
		float[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public double[] read(double[] l, int tag, boolean isRequire){
		double[] lr = null;
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
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
	
	public <T> T[] readArray(T[] l, int tag, boolean isRequire) {
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		if (l == null || l.length == 0)
			throw new JceDecodeException("unable to get type of key and value.");
		return readArrayImpl(l[0], tag, isRequire);
	}
	
	public <T> List<T> readArray(List<T> l, int tag, boolean isRequire) {
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		if (l == null || l.isEmpty()) {
			return new ArrayList<T>();
			//throw new TafProxyDecodeException("unable to get type of key and value.");
		}
		T[] tt = readArrayImpl(l.get(0), tag, isRequire);
		if(tt==null) return null;
		ArrayList<T> ll = new ArrayList<T>();
		for(int i = 0; i < tt.length; ++i)
			ll.add(tt[i]);
		return ll;
	}

	@SuppressWarnings("unchecked")
	private <T> T[] readArrayImpl(T mt, int tag, boolean isRequire) {
		if (skipToTag(tag)) {
			HeadData hd = new HeadData();
			readHead(hd);
			switch (hd.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new JceDecodeException("size invalid: " + size);
				T[] lr = (T[]) Array.newInstance(mt.getClass(), size);
				for (int i = 0; i < size; ++i) {
					T t = (T) read(mt, 0, true);
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

/*
	public <T> ArrayList<T> readList(List<T> l, int tag, boolean isRequire) {
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		if (l == null || l.isEmpty())
			throw new OneProxyDecodeException("unable to get type of key and value.");
		return (ArrayList<T>) readList(new ArrayList<T>(), l.get(0), tag, isRequire);
	}

	private <T> List<T> readList(List<T> lr, T mt, int tag, boolean isRequire) {
		//if (l == null || l.isEmpty())
		//	throw new OneProxyDecodeException("unable to get type of key and value.");
		// 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
		//T mt = l.get(0);
		if (skipToTag(tag)) {
			hd.clear();
			readHead(h);
			switch (h.type) {
			case JceStruct.LIST: {
				int size = read(0, 0, true);
				if (size < 0)
					throw new OneProxyDecodeException("size invalid: " + size);
				for (int i = 0; i < size; ++i) {
					T t = read(mt, 0, true);
					lr.add(t);
				}
				break;
			}
			case JceStruct.SIMPLE_LIST: {								
				HeadData hh = new HeadData();
				readHead(hh);
				if(hh.type != JceStruct.BYTE){
					throw new OneProxyDecodeException("type mismatch, tag: " + tag + ", type: " + h.type + ", " + hh.type);
				}
				int size = read(0, 0, true);
				if(size < 0)
					throw new OneProxyDecodeException("invalid size, tag: " + tag + ", type: " + h.type + ", " + hh.type + ", size: " + size);
				byte[] bytes = new byte[size];
				bs.get(bytes);
				for (int i = 0; i < size; ++i) {
					lr.add((T)Byte.valueOf(bytes[i]));
				}
				break;
			}
			default:
				throw new OneProxyDecodeException("type mismatch.");
			}
		} else if (isRequire) {
			throw new OneProxyDecodeException("require field not exist.");
		}
		return lr;
	}
*/
	
	public JceStruct directRead(JceStruct o, int tag, boolean isRequire) {
		//o必须有一个无参的构造函数
		JceStruct ref = null;
		if (skipToTag(tag)) {
			try{
				ref = o.newInit();
			}catch (Exception e){
				throw new JceDecodeException(e.getMessage());			
			}					
			
			HeadData hd = new HeadData();
			readHead(hd);
			if (hd.type != JceStruct.STRUCT_BEGIN)
				throw new JceDecodeException("type mismatch.");
			ref.readFrom(this);
			skipToStructEnd();
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return ref;
	}
	
	public JceStruct read(JceStruct o, int tag, boolean isRequire) {
		//o必须有一个无参的构造函数
		JceStruct ref = null;
		if (skipToTag(tag)) {
			try{
				ref = o.getClass().newInstance();
			}catch (Exception e){
				throw new JceDecodeException(e.getMessage());			
			}					
			
			HeadData hd = new HeadData();
			readHead(hd);
			if (hd.type != JceStruct.STRUCT_BEGIN)
				throw new JceDecodeException("type mismatch.");
			ref.readFrom(this);
			skipToStructEnd();
		} else if (isRequire) {
			throw new JceDecodeException("require field not exist.");
		}
		return ref;
	}

	public JceStruct[] read(JceStruct[] o, int tag, boolean isRequire) {
		return readArray(o, tag, isRequire);
	}
	
	@SuppressWarnings("unchecked")
	public <T> Object read(T o, int tag, boolean isRequire) {
		if (o instanceof Byte) {
			return Byte.valueOf(read((byte) 0x0, tag, isRequire));
		} else if (o instanceof Boolean) {
			return Boolean.valueOf(read(false, tag, isRequire));
		} else if (o instanceof Short) {
			return Short.valueOf(read((short) 0, tag, isRequire));
		} else if (o instanceof Integer) {
			int i = read((int) 0, tag, isRequire);
			return Integer.valueOf(i);
		} else if (o instanceof Long) {
			return Long.valueOf(read((long) 0, tag, isRequire));
		} else if (o instanceof Float) {
			return Float.valueOf(read((float) 0, tag, isRequire));
		} else if (o instanceof Double) {
			return Double.valueOf(read((double) 0, tag, isRequire));
		} else if (o instanceof String) {
			return String.valueOf(readString(tag, isRequire));
		} else if (o instanceof Map) {
			return readMap((Map) o, tag, isRequire);
		} else if (o instanceof List) {
			return readArray((List) o, tag, isRequire);
		} else if (o instanceof JceStruct) {
			return read((JceStruct) o, tag, isRequire);
		} else if (o.getClass().isArray()) {
			if(o instanceof byte[] || o instanceof Byte[]){
				return read((byte[]) null, tag, isRequire);
			}else if(o instanceof boolean[]){
				return read((boolean[]) null, tag, isRequire);
			}else if(o instanceof short[]){
				return read((short[]) null, tag, isRequire);
			}else if(o instanceof int[]){
				return read((int[]) null, tag, isRequire);
			}else if(o instanceof long[]){
				return read((long[]) null, tag, isRequire);
			}else if(o instanceof float[]){
				return read((float[]) null, tag, isRequire);
			}else if(o instanceof double[]){
				return read((double[]) null, tag, isRequire);
			}else{
				return readArray((Object[])o, tag, isRequire);
			}
		} else {
			throw new JceDecodeException("read object error: unsupport type.");
		}
	}

    protected String sServerEncoding = "UTF-8";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

	
	public static void main(String[] args) {

	}


	public ByteBuffer getBs() {
		return bs;
	}
}
