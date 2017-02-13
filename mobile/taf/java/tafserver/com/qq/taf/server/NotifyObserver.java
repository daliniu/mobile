package com.qq.taf.server;

import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.server.JceCurrent;

/**
 * 全局接收管理命令并进行分发处理
 * @author fanzhang
 */
public class NotifyObserver {
	
	private static final NotifyObserver INSTANCE=new NotifyObserver();
	private Map<String, Set<NotifyInvoke>> prefix;
	private Map<String, Set<NotifyInvoke>> notify;
	
	private NotifyObserver() {
		prefix=new HashMap<String, Set<NotifyInvoke>>();
		notify=new HashMap<String, Set<NotifyInvoke>>();
	}
	
	public static NotifyObserver getInstance() {
		return INSTANCE;
	}
	
	public String notify(String command,JceCurrent jc) {
		String params="";
		String comm=command;
		int i=command.indexOf(" ");
		if(i!=-1) {
			comm=command.substring(0,i);
			params=command.substring(i+1);
		}
		
		StringBuilder result=new StringBuilder();
		result.append("[ command ]\n"+command+"\n");
		
		if(prefix.containsKey(comm)) {
			int prefixnum=0;
			for(NotifyInvoke invoke:prefix.get(comm)) {
				JceStringHolder ret=new JceStringHolder();
				boolean goon=invoke.notify(comm, params, jc, ret);
				String result0=goon?ret.value:"BREAK";
				result.append("[ prefix_"+(++prefixnum)+" ]\n"+result0+"\n");
				if(!goon) return result.toString();
			}
		}
		
		if(notify.containsKey(comm)) {
			int notifynum=0;
			for(NotifyInvoke invoke:notify.get(comm)) {
				JceStringHolder ret=new JceStringHolder();
				boolean goon=invoke.notify(comm, params, jc, ret);
				String result0=goon?ret.value:"BREAK";
				result.append("[ notify_"+(++notifynum)+" ]\n"+result0+"\n");
				if(!goon) return result.toString();
			}
		}
		
		return result.toString();
	}
	
	public void registerPrefix(String command,NotifyInvoke invoke) {
		if(!prefix.containsKey(command)) prefix.put(command,new LinkedHashSet<NotifyInvoke>());
		prefix.get(command).add(invoke);
	}
	
	public void unregisterPrefix(String command,NotifyInvoke invoke) {
		if(prefix.containsKey(command)) prefix.get(command).remove(invoke);
	}
	
	public void registerNotify(String command,NotifyInvoke invoke) {
		if(!notify.containsKey(command)) notify.put(command,new LinkedHashSet<NotifyInvoke>());
		notify.get(command).add(invoke);
	}
	
	public void unregisterNotify(String command,NotifyInvoke invoke) {
		if(notify.containsKey(command)) notify.get(command).remove(invoke);
	}
	
}
