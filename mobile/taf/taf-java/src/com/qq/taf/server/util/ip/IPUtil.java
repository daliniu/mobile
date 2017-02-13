package com.qq.taf.server.util.ip;


/**
 * IP工具类，主要支持ip与正Long类型的转换,支持ipv4和ipv6
 * @author easezhang
 *
 */
public class IPUtil {
	private final static long MAX_IPV4_VALUE = 0x0FFFFFFFFL;
	private final static long[] MASKS = new long[]{0x00FF, 0x00FF00, 0x00FF0000, 0x00FF000000,0x00FF00000000L,0x00FF0000000000L};
	
	/**
	 * 将ip字符串转换为long，ip格式务必正确,不能包含非法字符
	 * @param ip
	 * @return
	 */
	public static long castIpToLong(String ip) {
		return castIpToLong(ip, false);
	}
	
	/**
	 * 将ip转换成long，并作格式检查
	 * @param ip
	 * @return
	 */
	public static long castIpToLongWithFormatCheck(String ip) {
		return castIpToLong(ip, true);
	}
	
	/**
	 * 将long转换为ip
	 * @param value
	 * @return
	 */
	public static String castLongToIp(long value) {
		StringBuffer sb = new StringBuffer();
		if(value>MAX_IPV4_VALUE) {
			sb.append((value & MASKS[5])>>>40);
			sb.append(".");
			sb.append((value & MASKS[4])>>>32);
			sb.append(".");
		}
		sb.append((value & MASKS[3])>>>24);
		sb.append(".");
		sb.append((value & MASKS[2])>>>16);
		sb.append(".");
		sb.append((value & MASKS[1])>>>8);
		sb.append(".");
		sb.append((value & MASKS[0]));
		return sb.toString();
	}
	
	/**
	 * 分割ip字符串，由于jutil的StringUtil.split有效率问题，修改为该函数后，效率提升4倍
	 * @param ip
	 * @param bs
	 * @return
	 * @throws Exception 
	 */
	private static int splitIpString(String ip, byte[] bs) throws Exception {
		int i=ip.length()-1;
		int tmp = 0;
		int radix = 1;
		int j = i;
		int len = 0;
		char tmpChar = '\0';
		int tmpValue = 0;
		while(i > -1) {
			tmp = 0;
			radix = 1;
			j=i;
			++len;
			while(j>-1) {
				tmpChar = ip.charAt(j);
				--j;
				if(tmpChar==' ') {
					continue;
				}
				if(tmpChar=='.') {
					bs[bs.length-len] = (byte)(tmp&0xFF);
					i=j;
					break;
				}
				tmpValue = tmpChar-'0';
				if(tmpValue<0||tmpValue>9) {
					throw new Exception("ip="+ip+" format error");
				}
				tmp+=radix*tmpValue;
				radix*=10;
				if(j==-1) {
					bs[bs.length-len] = (byte)(tmp&0xFF);
					i=j;
					break;
				}
			}
			
		}
		return len;
	}
	
	
	public static long castIpToLong(String ip, boolean isCheck) {
		try {
			if(isValidIp(ip, isCheck)) {
				byte []bs = new byte[6];
				int len = splitIpString(ip, bs);
				return getLongFromByteArray(bs, bs.length-len, len);
			}
			else {
				throw new RuntimeException("ip \""+ip+"\" format error");
			}
		}
		catch(Exception e) {
			return -1;
		}
	}
	
	
	
	private static boolean isValidIp(String ip, boolean isCheck) {
		try {
			if(!isCheck) {
				return true;
			}
			if(ip == null || ip.length()==0) {
				return false;
			}
			byte []bs = new byte[6];
			int tmp = splitIpString(ip, bs);
			return tmp ==6 || tmp ==4;
		}
		catch(Exception e) {
			throw new RuntimeException(e);
		}
	}
	
//	private static boolean isValidDomainCode(String code, boolean isCheck) {
//		if(!isCheck) {
//			return true;
//		}
//		if(code.length() == 0) {
//			return false;
//		}
//		int tmp = Integer.valueOf(code);
//		return tmp>=0&&tmp<=255;
//	}
//	
//	private static byte castDomainCodeToByte(String domainCode) {
//		return (byte)(Integer.valueOf(domainCode) & 0x00FF);
//	}
	
	
	public static long getLongFromByteArray(byte []bs, int offset, int len) {
		long result = 0L;
		for(int i=offset+len-1;i>=offset;i--) {
			result|= ((0L|(bs[i]&0x00ff))<<((offset+len-i-1)*8));
		}
		return result;
	}
	
	public static void main(String ...args) throws Exception {
		byte []bs = new byte[]{(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF};
		System.out.println(getLongFromByteArray(bs, 0 ,4));
		System.out.println(Integer.toHexString((int)(getLongFromByteArray(bs, 0, 4))));
		
		System.out.println("====================");
		bs = new byte[]{(byte)192,(byte)168,(byte)0,(byte)0};
		System.out.println(castIpToLong("192.168.0.0"));
		System.out.println(getLongFromByteArray(bs, 0 ,4));
		System.out.println(Integer.toHexString((int)(getLongFromByteArray(bs, 0 ,4))));
		System.out.println(castLongToIp(3232235520L));
		System.out.println(castLongToIp(MAX_IPV4_VALUE));
		System.out.println(MAX_IPV4_VALUE);
		System.out.println("====================");
		
		bs = new byte[]{1,2,3,4};
		System.out.println(getLongFromByteArray(bs, 0 ,4));
		System.out.println(Integer.toHexString((int)(getLongFromByteArray(bs, 0 ,4))));
		System.out.println("====================");
		String ip = "198.12.144.0";
		byte result[] = new byte[6];
		long a = castIpToLong(ip, true);
		int len = splitIpString(ip, result);
		for(int i=result.length-len;i<result.length;i++) {
			System.out.println(result[i]);
		}
	}
}
