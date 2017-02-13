package com.qq.java.testcase;

import static com.qq.taf.server.ApplicationUtil.*;
import com.qq.taf.server.Application;
import com.qq.taf.server.ServerConfig;

public class TestCase extends Application {

	@Override
	protected void initialize() throws Throwable {
		addServant(TafCaseServantImpl.class, ServerConfig.application+"."+ServerConfig.serverName+".TafCaseServant");
	}
	
	public static void main(String[] args) {
		new TestCase().start();
	}

}
