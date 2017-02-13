package com.qq.java.test;

import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

public class Test {

	public static void main(String[] args) {
		BrokerDisplayInfo bdi=new BrokerDisplayInfo();
		System.out.println(bdi.toString());
		JceOutputStream jos=new JceOutputStream();
		bdi.writeTo(jos);
		byte[] data=jos.toByteArray();
		System.out.println("===============");
		JceInputStream jis=new JceInputStream(data);
		BrokerDisplayInfo bdi2=new BrokerDisplayInfo();
		bdi2.readFrom(jis);
		System.out.println(bdi2.toString());
	}
	
}
