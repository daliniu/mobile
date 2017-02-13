package com.duowan.taf.proxy.utils;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigDecimal;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Properties;
import java.util.Map.Entry;
import java.util.concurrent.atomic.AtomicBoolean;

import com.duowan.taf.ClientInfo;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.proxy.Pair;
import com.duowan.taf.proxy.ProxyConfig;
import com.duowan.taf.proxy.TafLoggerCenter;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;
import com.duowan.taf.proxy.exec.TafProxyConfigException;

public class TafUtils {

	public final static String nodeDataFileName = "tafnode.dat";

	static String dataFilePath = nodeDataFileName;

	public static StatMicMsgHead getHead(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue) {
		masterName = masterName + "@" + ClientInfo.getClientNumber();
		StatMicMsgHead head = new StatMicMsgHead(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort,
				returnValue);
		return head;
	}

	static Properties props = new Properties();

	static AtomicBoolean propsInited = new AtomicBoolean();

	static long lastSaveTime = System.currentTimeMillis();

	static Object lock = new Object();

	public static synchronized void loadNodeData() {
		if (propsInited.get()) {
			return;
		}
		InputStream in = null;
		try {
			in = getDataInputStream();
			props.load(in);
			ArrayList<String> removeKey = new ArrayList<String>();
			for (Entry<Object, Object> entry : props.entrySet()) {
				if (entry.getKey().toString().startsWith("<")) {
					removeKey.add(entry.getKey().toString());
				}
			}
			for (String key : removeKey) {
				props.remove(key);
			}
			TafLoggerCenter.info("load " + dataFilePath);
			propsInited.set(true);
		} catch (Exception e) {
			e.printStackTrace();
			throw new TafProxyConfigException("read file " + dataFilePath + " error " + e);
		} finally {
			if (null != in) {
				try {
					in.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	/**
	 * 获取服务注册信息
	 * 
	 * @param serverName
	 * @return
	 */
	public static String getServerInfoData(String serverName) {
		loadNodeData();
		return props.getProperty(serverName);
	}

	public static Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> getServerInfoData(String serverName,
			int syncTimeoutMill, int asyncTimeoutMill, int serviceConnNum) {
		String backConfigData = TafUtils.getServerInfoData(serverName);
		if (null == backConfigData) {
			TafLoggerCenter.info("can not find cacheServerData " + serverName);
		} else {
			TafLoggerCenter.info("getConfigFromRegister failed, try to use backData");
			try {
				Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = ProxyConfig.parseEndPoingConfig(serverName,
						syncTimeoutMill, asyncTimeoutMill, serviceConnNum, backConfigData);
				return p;
			} catch (Exception e) {
				TafLoggerCenter.info("read cacheServerData " + serverName + " serviceList error " + backConfigData, e);
			}
		}
		return null;
	}

	/**
	 * 设置服务注册信息
	 * 
	 * @param serverName
	 * @param serverInfo
	 */
	public static void setServerInfoData(String serverName, String serverInfo) {
		loadNodeData();
		props.put(serverName, serverInfo);
		synchronized (lock) {
			if (System.currentTimeMillis() - lastSaveTime > 60000) {
				lastSaveTime = System.currentTimeMillis();
				saveProps();
			}
		}
	}

	private static InputStream getDataInputStream() throws IOException {
		File path = new File(getDataPath("TafUtils.class",TafUtils.class));
		if (!path.exists()) {
			path.mkdirs();
		}
		String separator = File.separator;
		if ( isWindows() ) {
			separator = "/";
		}
		File f = new File(path +separator+nodeDataFileName);
		if ( !f.exists() ) {
			f.createNewFile();
		}
		dataFilePath = f.getAbsolutePath();
		TafLoggerCenter.info("set dataFilePath "+dataFilePath);
		InputStream in = new BufferedInputStream(new FileInputStream(f));
		return in;
	}

	public static boolean isWindows() {
		return File.separatorChar == '\\' ;
	}
	
	/**
	 * 设定data文件位置 发现WEB-INF则放在WEB-INF/classes下
	 * @param resource
	 * @param clazz
	 * @return
	 */
	public static String getDataPath(String resource, Class clazz) {
		URL u = clazz.getResource(resource);
		String path = u.getPath();
		if ( path.startsWith("file:") ) {
			path = path.substring(path.indexOf("/"));
			int point = path.indexOf("!");
			if ( point > 0  ) {
				path = path.substring(0,point);
			}
		}
		if (isWindows() &&  path.startsWith("/") ) {
			path = path.substring(1);
		}
		char separator = File.separatorChar;
		if ( isWindows() ) {
			separator = '/';
		}
		int point = path.indexOf("WEB-INF");
		if ( point > 0 ) {
			path = path.substring(0,point+7)+separator+"classes"+separator;
		} 
		if ( path.charAt(path.length()-1) != separator ) {
			path = path.substring(0,path.lastIndexOf(separator)+1);
		}
		return path ;
	}

	public static void saveProps() {
		OutputStream out = null;
		try {
			loadNodeData();
			File f = new File(dataFilePath);
			if (!f.exists()) {
				f.createNewFile();
			}
			out = new BufferedOutputStream(new FileOutputStream(f));
			props.store(out, (new Date()).toString());
			TafLoggerCenter.info("save " + dataFilePath);
		} catch (Exception e) {
			e.printStackTrace();
			TafLoggerCenter.info("save " + dataFilePath + " failed", e);
		} finally {
			if (null != out) {
				try {
					out.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	public static void main(String[] args) {
		getDataPath("TafUtils.class",TafUtils.class);
	}

	public static String formatBytes(long num) {
		double returnValue = num;
		String tail = "B";
		int i = 0;
		while (returnValue > 1024) {
			returnValue = div(returnValue, 1024, 2);
			i++;
			if (i == 1) {
				tail = "K";
			} else if (i == 2) {
				tail = "M";
			} else if (i == 3) {
				tail = "G";
				break;
			}
		}
		return returnValue + tail;
	}

	private static final int DEF_DIV_SCALE = 10;

	/** */
	/**
	 * 提供（相对）精确的除法运算，当发生除不尽的情况时，精确到 小数点以后10位，以后的数字四舍五入。
	 * 
	 * @param v1
	 *            被除数
	 * @param v2
	 *            除数
	 * @return 两个参数的商
	 */
	public static double div(double v1, double v2) {
		return div(v1, v2, DEF_DIV_SCALE);
	}

	/** */
	/**
	 * 提供（相对）精确的除法运算。当发生除不尽的情况时，由scale参数指 定精度，以后的数字四舍五入。
	 * 
	 * @param v1
	 *            被除数
	 * @param v2
	 *            除数
	 * @param scale
	 *            表示表示需要精确到小数点以后几位。
	 * @return 两个参数的商
	 */
	public static double div(double v1, double v2, int scale) {
		if (scale < 0) {
			throw new IllegalArgumentException("The scale must be a positive integer or zero");
		}
		BigDecimal b1 = new BigDecimal(Double.toString(v1));
		BigDecimal b2 = new BigDecimal(Double.toString(v2));
		return b1.divide(b2, scale, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

	/** */
	/**
	 * 提供精确的小数位四舍五入处理。
	 * 
	 * @param v
	 *            需要四舍五入的数字
	 * @param scale
	 *            小数点后保留几位
	 * @return 四舍五入后的结果
	 */
	public static double round(double v, int scale) {
		if (scale < 0) {
			throw new IllegalArgumentException("The scale must be a positive integer or zero");
		}
		BigDecimal b = new BigDecimal(Double.toString(v));
		BigDecimal one = new BigDecimal("1");
		return b.divide(one, scale, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

}
