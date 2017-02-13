package com.qq.taf.server.util.ip;

import com.qq.jutil.string.StringUtil;



/**
 * 模糊ip域，例如192.168.*.*；
 * 此处不确定段只支持英文星号，而且只能满足后缀原则（*只能从后向前出现）
 * @author easezhang
 *
 */
class FuzzyIpDomain {
	private long begin;
	private long end;
	//private DomainIndex domainIndex;
	
	
	public FuzzyIpDomain(String ip) {
		begin = IPUtil.castIpToLongWithFormatCheck(StringUtil.replaceAll(ip, "*", "0"));
		end = IPUtil.castIpToLongWithFormatCheck(StringUtil.replaceAll(ip, "*", "255"));
		//domainIndex = new DomainIndex(begin, end);
	}
	
	public FuzzyIpDomain(long low, long high) {
		begin = low;
		end = high;
		//domainIndex = di;
	}
	
	public long getBegin() {
		return begin;
	}

	public long getEnd() {
		return end;
	}

	/**
	 * 两个ip域是否有交集
	 * @param index
	 * @return
	 */
	public boolean hasSameDomain(FuzzyIpDomain index) {
		return this.begin>=index.begin&&this.begin<=index.end ||
		this.end>=index.begin && this.end<=index.end||
		index.begin>=this.begin&&index.begin<=this.end||
		index.end>=this.begin && index.end<=this.end;
	}
	
	/**
	 * 求两个ip域之间的并集，只有两个集合连续或者存在交集的时候才能求并集
	 * @param index
	 * @return
	 */
	public FuzzyIpDomain getUnionDomain(FuzzyIpDomain index) {
		if(hasSameDomain(index)||isSerial(index)) {
			FuzzyIpDomain result = new FuzzyIpDomain(Math.min(this.begin, index.begin), Math.max(this.end, index.end));
			return result;
		}
		return null;
	}
	
	/**
	 * 两个ip域之间是否连续
	 * @param index
	 * @return
	 */
	public boolean isSerial(FuzzyIpDomain index) {
		return end+1==index.begin||index.end+1==begin;
	}

	public boolean isInDomain(long value) {
		return value>=begin && value<=end;
	}
	
	public String toString() {
		return IPUtil.castLongToIp(begin)+","+IPUtil.castLongToIp(end)+"|"+begin+","+end;
	}

	public boolean isGt(FuzzyIpDomain dom) {
		return this.begin > dom.end;
	}

	public boolean isLt(FuzzyIpDomain dom) {
		return this.end < dom.begin;
	}
	
	public static void main(String ...args) {
		String ip = "192.168.0.0";
		System.out.println(new FuzzyIpDomain(ip));
	}
}
