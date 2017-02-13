package com.qq.taf.server.util.ip;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.qq.jutil.string.StringUtil;

/**
 * ip列表，主要提供外部ip是否与名单内ip匹配的功能
 * 3Gallow_ip合并后655个ip，压测最差匹配100w/s
 * @author easezhang
 *
 */
public class IPList {
	
	private List<FuzzyIpDomain> domainIndexs;
	
	private long low;
	
	private long high;
	
	public IPList(String ipStr) {
		this((ipStr==null||ipStr.trim().length()==0)?null:Arrays.asList(StringUtil.replaceAll(ipStr, ";", ",").split(",")));
	}
	/**
	 * ip名单构造
	 * @param ipStr ip串
	 * @param splitor 分隔符，不支持正则
	 */
	public IPList(String ipStr, char splitor) {
		this(Arrays.asList(split(ipStr, splitor)));
	}
	
	public boolean isEmpty() {
		return domainIndexs==null||domainIndexs.isEmpty();
	}
	private static String[] split(String src, char seprator) {
		ArrayList<String> result = new ArrayList<String>();
		char tmpChar = '\0';
		StringBuffer sb = new StringBuffer();
		for(int i=0;i<src.length();i++) {
			tmpChar = src.charAt(i);
			if(tmpChar==seprator) {
				result.add(sb.toString());
				sb = new StringBuffer();
			}
			else {
				sb.append(tmpChar);
			}
		}
		if(sb.length()!=0) {
			result.add(sb.toString());
		}
		String [] strs = new String[result.size()];
		return result.toArray(strs);
	}
	
	/**
	 * ip名单构造
	 * @param ipList ip列表
	 */
	public IPList(List<String> ipList) {
		if(ipList == null||ipList.isEmpty()) {
			return;
		}
		domainIndexs = this.parseIpList(ipList);
		if(domainIndexs == null || domainIndexs.isEmpty()) {
			return;
		}
		this.low = domainIndexs.get(0).getBegin();
		this.high = domainIndexs.get(domainIndexs.size()-1).getEnd();
	}
	
	private List<FuzzyIpDomain> parseIpList(List<String> ipList) {
		List<FuzzyIpDomain> result = new ArrayList<FuzzyIpDomain>(ipList.size());
		for(String ip:ipList) {
			if(ip.length()==0||ip.trim().length()==0) {
				continue;
			}
			FuzzyIpDomain dom = null;
			dom = new FuzzyIpDomain(ip);
			this.addIPDomain(dom, result);
		}
		result = combineDomainIndex(result);
		return result;
	}
	
	private void addIPDomain(FuzzyIpDomain dom, List<FuzzyIpDomain> scope) {
		if(scope.isEmpty()) {
			scope.add(dom);
		}
		else {
			int low = 0;
			int high = scope.size()-1;
			int tmp = (low+high)/2;
			if(dom.isLt(scope.get(low))) {
				scope.add(0, dom);
				return;
			}
			else if(dom.isGt(scope.get(high))) {
				scope.add(dom);
				return;
			}
			while(low<=high) {
				if(dom.isGt(scope.get(tmp))) {
					if(dom.isLt(scope.get(tmp+1))) {
						scope.add(tmp+1, dom);
						break;
					}
					low = tmp+1;
					tmp = (low+high)/2;
				}
				else if(dom.isLt(scope.get(tmp))) {
					if(dom.isGt(scope.get(tmp-1))) {
						scope.add(tmp, dom);
						break;
					}
					high = tmp-1;
					tmp = (low+high)/2;
				}
				else if(dom.hasSameDomain(scope.get(tmp))) {
					scope.add(tmp, dom.getUnionDomain(scope.get(tmp)));
					break;
				}
			}
		}
	}
	
	private List<FuzzyIpDomain> combineDomainIndex(List<FuzzyIpDomain> doms) {
		ArrayList<FuzzyIpDomain> result = new ArrayList<FuzzyIpDomain>(doms.size());
		int i=0;
		FuzzyIpDomain tmpDom = null;
		while(i<doms.size()) {
			int tmp = i+1;
			FuzzyIpDomain index = doms.get(i);
			while(tmp<doms.size()) {
				tmpDom = index.getUnionDomain(doms.get(tmp));
				if(tmpDom == null) {
					break;
				}
				else {
					index = tmpDom;
					++tmp;
				}
			}
			doms.set(i, index);
			result.add(doms.get(i));
			i = tmp;
		}
		return result;
	}
	
	private boolean contains(long tmpIp) {
		if(domainIndexs == null||domainIndexs.isEmpty()||tmpIp<0) {
			return false;
		}
		if(tmpIp < this.low || tmpIp>this.high) {
			return false;
		}
		int low = 0;
		int high = domainIndexs.size()-1;
		int tmp = (low+high)/2;
		while(low<=high) {
			if(domainIndexs.get(tmp).isInDomain(tmpIp)) {
				return true;
			}
			else if(domainIndexs.get(tmp).getBegin() > tmpIp) {
				high = tmp-1;
				tmp = (low+high)/2;
			}
			else if(domainIndexs.get(tmp).getEnd() < tmpIp) {
				low = tmp+1;
				tmp = (low+high)/2;
			}
			if(low==high) {
				if(domainIndexs.get(tmp).isInDomain(tmpIp)) {
					return true;
				}
				break;
			}
		}
		return false;
	}
	/**
	 * ip列表是否包含指定ip
	 * @param ip ip字符串
	 * @return
	 */
	public boolean contains(String ip) {
		long tmpIp = IPUtil.castIpToLong(ip);
		return contains(tmpIp);
	}
	
	public boolean contains(InetAddress addr) {
		if(addr == null) {
			return false;
		}
		byte []bs = addr.getAddress();
		return contains(IPUtil.getLongFromByteArray(bs, 0, bs.length));
	}
	
	public String toString() {
		if(this.isEmpty()) {
			return "ip list is empty";
		}
		StringBuilder sb = new StringBuilder("ip list is:");
		for(FuzzyIpDomain ip : domainIndexs) {
			sb.append(ip);
			sb.append("|");
			sb.append("\n");
		}
		return sb.toString();
	}
	
	public static void main(String ...args) throws Throwable {
		IPList list1 = new IPList("192.168.1.1 ;192.168.2.*;192.168.*.*;192.167.0.1", ';');
		IPList list = new IPList("");
		//System.out.println("size="+list.domainIndexs.size());
		//System.out.println(list.domainIndexs.get((list.domainIndexs.size()/2+1)));
		//System.out.println(list.domainIndexs.get((list.domainIndexs.size()/2)));
//		for(FuzzyIpDomain di:list.domainIndexs) {
//			System.out.println(di);
//		}
//		System.out.println(list.contains("8.12.144.1"));
//		long begin = System.currentTimeMillis();
		for(int i=0;i<10000000;i++) {
			System.out.println(list.contains("8.12.144.1"));
		}
		//System.out.println(System.currentTimeMillis()-begin);
	}
	
	private static ArrayList<String> readFile() throws IOException {
		File f = new File("e:/down/test1");
		BufferedReader reader = new BufferedReader(new FileReader(f));
		String tmp = null;
		ArrayList<String> result = new ArrayList<String>();
		while((tmp=reader.readLine())!=null) {
			result.add(tmp);
		}
		System.out.println(result);
		return result;
	}
}
