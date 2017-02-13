package com.qq.taf.server;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * 通讯端点值对象
 * @author fanzhang
 */
public class Endpoint {

	private String str;
	private String protocol;
	private Map<String,String> values;
	
	public Endpoint(String str) {
		this.str=str;
		values=new HashMap<String, String>();
		Iterator<String> iter=Arrays.asList(str.split(" ")).iterator();
		if(iter.hasNext()) {
			protocol=iter.next();
			while(iter.hasNext()) values.put(iter.next(),iter.next());
		}
		if(!values.containsKey("-g")) values.put("-g", "0");
		if(!values.containsKey("-t")) values.put("-t", "60000");
	}
	
	public String getStringParam(String key) {
		return values.get(key);
	}
	
	public int getIntParam(String key) {
		return Integer.parseInt(values.get(key));
	}
	
	public String getProtocol() {
		return protocol;
	}
	
	public String getHost() {
		return getStringParam("-h");
	}
	
	public int getPort() {
		return getIntParam("-p");
	}
	
	public int getTimeout() {
		return getIntParam("-t");
	}
	
	public int getGrid() {
		return getIntParam("-g");
	}
	
	@Override
	public String toString() {
		return str;
	}
	
}
