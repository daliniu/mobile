package com.qq.java.test;

import com.qq.taf.server.Application;
import com.qq.taf.server.ServerConfig;

public class Test extends Application {

	@Override
	protected void initialize() throws Throwable {
		addServant(BasicImpl.class, ServerConfig.application+"."+ServerConfig.serverName+".Basic");
	}
	
	public static void main(String[] args) {
		new Test().start();
	}

}
