package com.qq.java.test;

public class Tester extends Thread{
	public Tester(String name) {
		setName(name);
	}
	@Override
	public void run() {
		try {
			while(true) {
				TestClient.test();
				TestClient.testOneWay();
				Thread.sleep(1);
			}
		}
		catch(Throwable e) {
			e.printStackTrace();
		}
	}
	
}
