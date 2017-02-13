package com.duowan.taf.proxy.exec;

import com.duowan.taf.proxy.SendMessage;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;

public class TafNotSendException extends TafException{

	ServiceEndPointInfo config;
	SendMessage msg;
	
	public TafNotSendException(ServiceEndPointInfo config, SendMessage msg) {
		super(msg.getFuture().getSServantName()+" not send at "+config);
		this.config = config ;
		this.msg = msg ;
	}

	public ServiceEndPointInfo getConfig() {
		return config;
	}

	public SendMessage getMsg() {
		return msg;
	}

}
