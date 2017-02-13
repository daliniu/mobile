package com.qq.taf.jce;


import java.util.Hashtable;

public final class JceDisplayer {
//	private StringBuffer sb;
//	private int _level = 0;
//
//	private void ps(String fieldName) {
//		for (int i = 0; i < _level; ++i)
//			sb.append('\t');
//		if (fieldName != null)
//			sb.append(fieldName).append(": ");
//	}
//
//	public JceDisplayer(StringBuffer sb, int level) {
//		this.sb = sb;
//		this._level = level;
//	}
//
//	public JceDisplayer(StringBuffer sb) {
//		this.sb = sb;
//	}
//
//	public JceDisplayer display(boolean b, String fieldName) {
//		ps(fieldName);
//		sb.append(b ? 'T' : 'F').append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(byte n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(char n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(short n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(int n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(long n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}

//	public JceDisplayer display(float n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(double n, String fieldName) {
//		ps(fieldName);
//		sb.append(n).append('\n');
//		return this;
//	}

//	public JceDisplayer display(String s, String fieldName) {
//		ps(fieldName);
//		sb.append(s).append('\n');
//		return this;
//	}
//
//	public JceDisplayer display(byte[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			byte o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}
//
//	public JceDisplayer display(char[] v, String fieldName) {
//		ps(fieldName); 
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			char o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}
//
//	public JceDisplayer display(short[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			short o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}
//
//	public JceDisplayer display(int[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			int o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}
//
//	public JceDisplayer display(long[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			long o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}

//	public JceDisplayer display(float[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			float o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}
//
//	public JceDisplayer display(double[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			double o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}

//	public JceDisplayer display(Hashtable m, String fieldName) {
//		ps(fieldName);
//		if (m.isEmpty()) {
//			sb.append(m.size()).append(", {}").append('\n');
//			return this;
//		}
//		sb.append(m.size()).append(", {").append('\n');
//		JceDisplayer jd1 = new JceDisplayer(sb, _level + 1);
//		JceDisplayer jd = new JceDisplayer(sb, _level + 2);
//                java.util.Enumeration keys = m.keys();
//                while (keys.hasMoreElements()) {
//                    String key = (String)keys.nextElement();
//                    Object value = m.get(key);
//                    jd1.display('(', null);
//			jd.display(key, null);
//			jd.display(value, null);
//			jd1.display(')', null);
//                }
//		display('}', null);
//		return this;
//	}
//
//	public JceDisplayer display(Object[] v, String fieldName) {
//		ps(fieldName);
//		if (v.length == 0) {
//			sb.append(v.length).append(", []").append('\n');
//			return this;
//		}
//		sb.append(v.length).append(", [").append('\n');
//		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
//		for (int i = 0; i < v.length; i++) {
//			Object o = v[i];
//			jd.display(o, null);
//		}
//		display(']', null);
//		return this;
//	}



//	public JceDisplayer display(Object o, String fieldName) {
//		if (o instanceof Byte) {
//			display(((Byte) o).byteValue(), fieldName);
//		} else if (o instanceof Boolean) {
//			display(((Boolean) o).booleanValue(), fieldName);
//		} else if (o instanceof Short) {
//			display(((Short) o).shortValue(), fieldName);
//		} else if (o instanceof Integer) {
//			display(((Integer) o).intValue(), fieldName);
//		} else if (o instanceof Long) {
//			display(((Long) o).longValue(), fieldName);
//		} 
//		else if (o instanceof Float) {
//			display(((Float) o).floatValue(), fieldName);
//		} else if (o instanceof Double) {
//			display(((Double) o).doubleValue(), fieldName);
//		}
//		else if (o instanceof String) {
//			display((String) o, fieldName);
//		} else if (o instanceof Hashtable) {
//			display((Hashtable) o, fieldName);
//		}  else if (o instanceof JceStruct) {
//			display((JceStruct) o, fieldName);
//		} else if (o instanceof byte[]) {
//			display((byte[]) o, fieldName);
//		} else if (o instanceof boolean[]) {
//			display((boolean[]) o, fieldName);
//		} else if (o instanceof short[]) {
//			display((short[]) o, fieldName);
//		} else if (o instanceof int[]) {
//			display((int[]) o, fieldName);
//		} else if (o instanceof long[]) {
//			display((long[]) o, fieldName);
//		} 
//		else if (o instanceof float[]) {
//			display((float[]) o, fieldName);
//		} else if (o instanceof double[]) {
//			display((double[]) o, fieldName);
//		}
//		else if (o.getClass().isArray()) {
//			display((Object[]) o, fieldName);
//		} else {
//			throw new JceEncodeException("write object error: unsupport type.");
//		}
//		return this;
//	}

//	public JceDisplayer display(JceStruct v, String fieldName) {
//		display('{', fieldName);
//		v.display(sb, _level + 1);
//		display('}', null);
//		return this;
//	}

//	public static void main(String[] args) {
//		StringBuffer sb = new StringBuffer();
//		sb.append(1.2);
//		System.out.println(sb.toString());
//	}
}
