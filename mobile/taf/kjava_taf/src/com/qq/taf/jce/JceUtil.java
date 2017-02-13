package com.qq.taf.jce;

//import java.nio.ByteBuffer;
import java.util.Vector;

public final class JceUtil {
	public static boolean equals(boolean l, boolean r) {
		return l == r;
	}

	public static boolean equals(byte l, byte r) {
		return l == r;
	}

	public static boolean equals(char l, char r) {
		return l == r;
	}

	public static boolean equals(short l, short r) {
		return l == r;
	}

	public static boolean equals(int l, int r) {
		return l == r;
	}

	public static boolean equals(int l, Object r) {
		return false;
	}

	public static boolean equals(long l, long r) {
		return l == r;
	}

//	public static boolean equals(float l, float r) {
//		return l == r;
//	}
//
//	public static boolean equals(double l, double r) {
//		return l == r;
//	}

	public static boolean equals(Object l, Object r) {
		return l.equals(r);
	}

//	public static int compareTo(boolean l, boolean r) {
//		return (l ? 1 : 0) - (r ? 1 : 0);
//	}
//
//	public static int compareTo(byte l, byte r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(char l, char r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(short l, short r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(int l, int r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(long l, long r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(float l, float r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(double l, double r) {
//		return l < r ? -1 : (l > r ? 1 : 0);
//	}
//
//	public static int compareTo(Comparable l, Comparable r) {
//		return l.compareTo(r);
//	}
//
//	public static int compareTo(Comparable[] l, Comparable[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = l[li].compareTo(r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(boolean[] l, boolean[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(byte[] l, byte[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(char[] l, char[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(short[] l, short[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(int[] l, int[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(long[] l, long[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(float[] l, float[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}
//
//	public static int compareTo(double[] l, double[] r) {
//		for (int li = 0, ri = 0; li < l.length && ri < r.length; ++li, ++ri) {
//			int n = compareTo(l[li], r[ri]);
//			if (n != 0)
//				return n;
//		}
//		return compareTo(l.length, r.length);
//	}

	public static int hashCode(boolean o) {
		return o ? 1 : 0;
	}

	public static int hashCode(byte o) {
		return o;
	}

	public static int hashCode(char o) {
		return o;
	}

	public static int hashCode(short o) {
		return o;
	}

	public static int hashCode(int o) {
		return o;
	}

	public static int hashCode(long o) {
		return (int) o;
	}

//	public static int hashCode(float o) {
//		return new Float(o).hashCode();
//	}
//
//	public static int hashCode(double o) {
//		return new Double(o).hashCode();
//	}

	public static int hashCode(Object o) {
		return o.hashCode();
	}

	public static Vector reverse(Vector listTpye) {
		Vector temp = new Vector(listTpye.size());
		int point = listTpye.size() - 1;
		for (int i = point; i >= 0; i--) {
			temp.addElement(listTpye.elementAt(i));
		}
		return temp;
	}

//	private static final byte[] highDigits;
//
//	private static final byte[] lowDigits;
//
//	// initialize lookup tables
//	static {
//		final byte[] digits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
//
//		int i;
//		byte[] high = new byte[256];
//		byte[] low = new byte[256];
//
//		for (i = 0; i < 256; i++) {
//			high[i] = digits[i >>> 4];
//			low[i] = digits[i & 0x0F];
//		}
//
//		highDigits = high;
//		lowDigits = low;
//	}

//	public static String getHexdump(byte[] array) {
//		return getHexdump(ByteBuffer.wrap(array));
//	}
//
//	public static String getHexdump(ByteBuffer in) {
//		int size = in.remaining();
//		if (size == 0) {
//			return "empty";
//		}
//		StringBuffer out = new StringBuffer((in.remaining() * 3) - 1);
//		int mark = in.position();
//		// fill the first
//		int byteValue = in.get() & 0xFF;
//		out.append((char) highDigits[byteValue]);
//		out.append((char) lowDigits[byteValue]);
//		size--;
//		// and the others, too
//		for (; size > 0; size--) {
//			out.append(' ');
//			byteValue = in.get() & 0xFF;
//			out.append((char) highDigits[byteValue]);
//			out.append((char) lowDigits[byteValue]);
//		}
//		in.position(mark);
//		return out.toString();
//	}

}
