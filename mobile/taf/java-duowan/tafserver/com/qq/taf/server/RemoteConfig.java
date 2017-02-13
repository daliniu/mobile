package com.qq.taf.server;

import java.io.File;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.TimerProvider;
import com.qq.taf.server.jce2java.ConfigPrx;
import com.qq.taf.server.jce2java.ConfigPrxHelper;
import com.qq.taf.server.util.FileUtil;
import com.qq.taf.server.util.GeneralException;

/**
 * 从配置中心获取远程配置 
 * @author fanzhang
 */
public class RemoteConfig {
	
	private static final RemoteConfig INSTANCE=new RemoteConfig();
	private Communicator comm;
	private ConfigPrx configPrx;
	private String app;
	private String serverName;
	private String basePath;
	private int maxBakNum;
	
	private RemoteConfig() {
	}
	
	public static RemoteConfig getInstance() {
		return INSTANCE;
	}
	
	public int setConfigInfo(Communicator comm,String obj,String app,String serverName,String basePath,int maxBakNum) {
		this.comm=comm;
		this.configPrx=this.comm.stringToProxy(obj, ConfigPrxHelper.class);
		this.app=app;
		this.serverName=serverName;
		this.basePath=basePath;
		this.maxBakNum=maxBakNum;
		return 0;
	}
	
	public int setConfigInfo(Communicator comm,String obj,String app,String serverName,String basePath) {
		return setConfigInfo(comm, obj, app, serverName, basePath,5);
	}
	
	public synchronized boolean addConfig(String fileName,JceStringHolder result,boolean appConfigOnly) {
		boolean ok=true;
		String sresult="RemoteConfig|add config success|"+fileName;
		try {
			String fullFileName=basePath+"/"+fileName;
			String newFile=getRemoteFile(fileName, appConfigOnly);
			if(new File(fullFileName).exists() && !FileUtil.readFileToString(fullFileName).equals(FileUtil.readFileToString(newFile))) {
				for(int i=maxBakNum-1;i>=1;i--) {
					String bakFileI=index2file(fullFileName, i);
					if(new File(bakFileI).canRead()) localRename(bakFileI, index2file(fullFileName, i+1));
				}
				if(new File(fullFileName).canRead()) localRename(fullFileName, index2file(fullFileName, 1));
			}
			localRename(newFile, fullFileName);
		} catch(Exception e) {
			sresult="RemoteConfig|add config fail|"+ fileName + "|" + e.getMessage();
			ok=false;
		}
		result.value=sresult;
		return ok;
	}
	
	public boolean addConfig(String fileName,JceStringHolder result) {
		return addConfig(fileName, result,false);
	}
	
	private String getRemoteFile(String fileName, boolean appConfigOnly) {
		JceStringHolder config=new JceStringHolder();
		int ret=configPrx.loadConfig(app, appConfigOnly?"":serverName, fileName, config);
		String stream=config.value;
		if(ret!=0 || stream==null || stream.length()==0) GeneralException.raise("RemoteConfig|remote config file is empty|" + fileName);
		String newFile =basePath + "/" + fileName + "." + TimerProvider.getNow();
		FileUtil.writeStringToFile(newFile, stream);
		return newFile;
	}

	private String recoverSysConfig(String fullFileName) {
		String result="RemoteConfig|recover fail|no backup file.";
		try {
			for(int i=1;i<maxBakNum;i++) {
				String fileName=index2file(fullFileName, i);
				if(new File(fileName).canRead()) {
					localRename(fileName, fullFileName);
					result="RemoteConfig|recover success|" + fileName;
					break;
				}
			}
		} catch(Exception e) {
			result="RemoteConfig|recover fail|" + e.getMessage();
		}
		return result;
	}

	private String index2file(String fullFileName, int index) {
		return fullFileName+"."+index+".bak";
	}

	private void localRename(String oldFile, String newFile) {
		File srcFile=new File(oldFile);
		File destFile=new File(newFile);
		boolean ok=(!destFile.exists() || destFile.delete()) && srcFile.renameTo(destFile);
		if(!ok) GeneralException.raise("RemoteConfig|rename file error|" + oldFile + "|" + newFile);
	}
	
}
