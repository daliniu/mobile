package com.qq.taf.server.ahttpclient;


public class HttpServiceInfo {

	String serviceUrl;
	String host ;
	int port;
	
	public HttpServiceInfo(String serviceUrl) {
		this.serviceUrl = serviceUrl;
		try {
			splitHttpUrl(serviceUrl);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public void splitHttpUrl(String serviceUrl) {
		int point = serviceUrl.indexOf("//");
		int point2 = -1;
		if ( point == -1 ) {
			point2 = serviceUrl.indexOf("/");
		} else {
			point2 = serviceUrl.indexOf("/",point+2);
		}
		if ( point2 > 0 ) {
			if (point == -1) {
				host = serviceUrl.substring(0,point2);
			} else {
				host = serviceUrl.substring(point+2, point2);
			}
			int point3 = host.indexOf(":");
			if (point3 > 0 ) {
				port = Integer.parseInt(host.substring(point3+1));
				host = host.substring(0,point3);
			} else {
				port = 80;
			}
		} else {
			if (point == -1) {
				host = serviceUrl;
			} else {
				host = serviceUrl.substring(point+2);
			}
			int point3 = host.indexOf(":");
			if (point3 > 0 ) {
				port = Integer.parseInt(host.substring(point3+1));
				host = host.substring(0,point3);
			} else {
				port = 80;
			}
		}
	}
	
	public static void main(String[] args) {
		String url = "http://172.23.31.21/cgi-bin/eca/realtime.pl";
		HttpServiceInfo info = new HttpServiceInfo(url);
//		System.out.println(info.host+":"+info.port);
//		url = "http://172.23.31.21:8080/cgi-bin/eca/realtime.pl";
//		info = new HttpServiceInfo(url);
//		System.out.println(info.host+":"+info.port);
//		url = "172.23.31.21:8080/cgi-bin/eca/realtime.pl";
//		info = new HttpServiceInfo(url);
//		System.out.println(info.host+":"+info.port);
		url = "http://www.oa.com";
		info = new HttpServiceInfo(url);
		System.out.println(info.host+":"+info.port);
		url = "www.oa.com:8080";
		info = new HttpServiceInfo(url);
		System.out.println(info.host+":"+info.port);
	}
}
