package com.qq.taf.server;

import java.net.InetSocketAddress;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;

import com.qq.jutil.string.StringUtil;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.TAF_VERSION;

/**
 * 全局管理命令
 * @author fanzhang
 */
public class AdminCommands {
	
	private static final String TAF_CMD_LOAD_CONFIG="taf.loadconfig";        
	private static final String TAF_CMD_SET_LOG_LEVEL="taf.setloglevel";      
	private static final String TAF_CMD_VIEW_STATUS="taf.viewstatus";        
	private static final String TAF_CMD_VIEW_VERSION="taf.viewversion";       
	private static final String TAF_CMD_CONNECTIONS="taf.connection";        
	private static final String TAF_CMD_LOAD_PROPERTY="taf.loadproperty";      
	private static final String TAF_CMD_VIEW_ADMIN_COMMANDS="taf.help";              
	private static final String TAF_CMD_SET_DYEING="taf.setdyeing";   
	
	private static void log(String command,String params) {
		SystemLogger.record("ADMIN COMMAND ACCEPTED|"+command+"|"+params);
	}
	
	public static void load() {
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_VIEW_STATUS, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				StringBuilder sb=new StringBuilder();
				sb.append("----- Server -----\n").append(ServerConfig.application+"."+ServerConfig.serverName+"\n");
				for(AdapterConfig ac:AdapterConfig.values()) {
					Servant servant=Application.INSTANCE.getServant(ac.getServant());
					sb.append("----- Adapter -----\n")
						.append(String.format("servant = %s\n", ac.getServant()))
						.append(String.format("endpoint = %s\n", ac.getEndpoint().toString()))
						.append(String.format("maxconns = %d\n", ac.getMaxConns()))
						.append(String.format("threads = %d\n", ac.getThreads()))
						.append(String.format("queuecap = %d\n", ac.getQueueCapacity()))
						.append(String.format("queuetimeout = %d\n", ac.getQueueTimeout()))
						.append(String.format("queuesize = %d\n",servant.getQueueSize()))
						.append(String.format("totalconns = %d\n",servant.getSessionCount()));
				}
				result.value=sb.toString();
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_VIEW_VERSION, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				result.value=TAF_VERSION.VERSION;
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_CONNECTIONS, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				SimpleDateFormat sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
				StringBuilder sb=new StringBuilder();
				Map<InetSocketAddress, Map<InetSocketAddress,Long>> sessionStatus=SessionChecker.INSTANCE.getSessionStatus();
				for(InetSocketAddress bindAddress:sessionStatus.keySet()) {
					Map<InetSocketAddress,Long> sessionStauts0=sessionStatus.get(bindAddress);
					sb.append(String.format("----- %s -----\n", bindAddress.toString()));
					sb.append(String.format("total = %d\n",sessionStauts0.size()));
					for(InetSocketAddress remoteAddress:sessionStauts0.keySet()) 
						sb.append(String.format("%s = %s\n",remoteAddress.toString(),sdf.format(new Date(sessionStauts0.get(remoteAddress))))); 
				}
				result.value=sb.toString();
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_VIEW_ADMIN_COMMANDS, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				StringBuilder sb=new StringBuilder();
				sb.append(TAF_CMD_VIEW_STATUS).append("\n")
					.append(TAF_CMD_VIEW_VERSION).append("\n")
					.append(TAF_CMD_CONNECTIONS).append("\n")
					.append(TAF_CMD_VIEW_ADMIN_COMMANDS).append("\n")
					.append(TAF_CMD_LOAD_CONFIG).append("\n");
				result.value=sb.toString();
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_LOAD_CONFIG, new NotifyInvoke() {
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				String fileName=params.trim();
				RemoteConfig.getInstance().addConfig(fileName, result);
				RemoteNotify.getInstance().report(result.value);
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_SET_LOG_LEVEL, new NotifyInvoke() {
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command, params);
				String level=params.trim();
				LoggerFactory.getInstance().setLevel(level);
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_SET_DYEING, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command,params);
				StringBuilder sb=new StringBuilder();
				String[] array=StringUtil.split(params, " ");
				if(array.length==2 || array.length==3) {
					DyeingManager.getInstance().setDyeing(array[0], array[1], array.length==3?array[2]:"");
					sb.append("DyeingKey=").append(array[0]).append("\n")
						.append("DyeingServant=").append(array[1]).append("\n")
						.append("DyeingInterface=").append(array.length==3?array[2]:"").append("\n");
				} else {
					sb.append("Invalid parameters. Should be: dyeingKey dyeingServant [dyeingInterface]");
				}
				result.value=sb.toString();
				return true;
			}
		});
		
		NotifyObserver.getInstance().registerPrefix(TAF_CMD_LOAD_PROPERTY, new NotifyInvoke(){
			public boolean notify(String command, String params, JceCurrent jc, JceStringHolder result) {
				log(command,params);
				result.value="ERROR: This command has not been supported yet!";
				return true;
			}
		});
	}

}
