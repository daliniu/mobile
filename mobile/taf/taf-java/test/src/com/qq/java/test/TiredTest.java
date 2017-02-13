package com.qq.java.test;

public class TiredTest {
	public static void main(String ...args) {
		Tester[] ts = new Tester[4];
		for(int i=0;i<ts.length;i++) {
			new Tester("TiredTest-"+i).start();
		}
		
	}
}
