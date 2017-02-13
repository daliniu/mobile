package com.duowan.jce.wup;

import java.io.InputStream;
import java.util.Properties;

/**
 * Simple utility module to make it easy to plug in the server identifier when
 * integrating Tomcat.
 * 
 * @author Craig R. McClanahan
 * @version $Revision: 467222 $ $Date: 2006-10-24 11:17:11 +0800 (Tue, 24 Oct
 *          2006) $
 */

public class WupInfo {

	// ------------------------------------------------------- Static Variables

	/**
	 * The server information String with which we identify ourselves.
	 */
	private static String clientInfo = null;

	/**
	 * The server built String.
	 */
	private static String clientBuilt = null;

	/**
	 * The server's version number String.
	 */
	private static String clientNumber = null;

	static {

		try {
			InputStream is = WupInfo.class.getResourceAsStream("/com/duowan/jce/wup/wup.properties");
			Properties props = new Properties();
			props.load(is);
			is.close();
			clientInfo = props.getProperty("client.info");
			clientBuilt = props.getProperty("client.built");
			clientNumber = props.getProperty("client.number");
		} catch (Throwable t) {
			;
		}
		if (clientInfo == null)
			clientInfo = "Tencent Taf";
		if (clientBuilt == null)
			clientBuilt = "unknown";
		if (clientNumber == null)
			clientNumber = "unknown";

	}

	// --------------------------------------------------------- Public Methods

	/**
	 * Return the server identification for this version of Tomcat.
	 */
	public static String getClientInfo() {

		return (clientInfo);

	}

	/**
	 * Return the server built time for this version of Tomcat.
	 */
	public static String getClientBuilt() {

		return (clientBuilt);

	}

	/**
	 * Return the server's version number.
	 */
	public static String getClientNumber() {

		return (clientNumber);

	}
	
	public static String showString() {
		StringBuffer sb = new StringBuffer();
		sb.append("Client version: " + getClientInfo()+"\n");
		sb.append("Client built:   " + getClientBuilt()+"\n");
		sb.append("Client number:  " + getClientNumber()+"\n");
		sb.append("OS Name:        " + System.getProperty("os.name")+"\n");
		sb.append("OS Version:     " + System.getProperty("os.version")+"\n");
		sb.append("Architecture:   " + System.getProperty("os.arch")+"\n");
		sb.append("JVM Version:    " + System.getProperty("java.runtime.version")+"\n");
		sb.append("JVM Vendor:     " + System.getProperty("java.vm.vendor")+"\n");
		return sb.toString();
	}

	public static void main(String args[]) {
		System.out.println(showString());
		System.out.println("Client version: " + getClientInfo());
		System.out.println("Client built:   " + getClientBuilt());
		System.out.println("Client number:  " + getClientNumber());
		System.out.println("OS Name:        " + System.getProperty("os.name"));
		System.out.println("OS Version:     " + System.getProperty("os.version"));
		System.out.println("Architecture:   " + System.getProperty("os.arch"));
		System.out.println("JVM Version:    " + System.getProperty("java.runtime.version"));
		System.out.println("JVM Vendor:     " + System.getProperty("java.vm.vendor"));
	}

}
