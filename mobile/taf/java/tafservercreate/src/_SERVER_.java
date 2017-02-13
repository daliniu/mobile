package com.qq._APPLOWWER_._SERVERLOWWER_;

import static com.qq.taf.server.ApplicationUtil.*;
import com.qq.taf.server.Application;
import com.qq.taf.server.ServerConfig;

public class _SERVER_ extends Application {

	@Override
	protected void initialize() throws Throwable {
		addServant(_SERVANT_Impl.class, ServerConfig.application+"."+ServerConfig.serverName+"._SERVANT_");
	}
	
	public static void main(String[] args) {
		new _SERVER_().start();
	}

}
