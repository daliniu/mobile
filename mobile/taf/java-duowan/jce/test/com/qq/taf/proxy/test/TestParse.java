package com.qq.taf.proxy.test;

import com.qq.taf.proxy.CommunicatorConfig;

public class TestParse {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		CommunicatorConfig config = CommunicatorConfig.parseQueryConfig("D:\\myeclipse\\workspace\\taf\\test\\server\\server.config.conf");
		System.out.println(config.toString());
	}

}
