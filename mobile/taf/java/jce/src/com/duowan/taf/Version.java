package com.duowan.taf;

public class Version {
	
	static String major = "1";
	static String minor = "1";
	static String build = "5";
	static String path = "0";
	
	
	public static String getVersionInfo() {
		return major+""+minor+""+build+""+path;
	}
	
	
	public String toString() {
		return getVersionInfo();
	}
	
	public static void main(String[] args) {
		System.out.println(Version.getVersionInfo());
	}
}
