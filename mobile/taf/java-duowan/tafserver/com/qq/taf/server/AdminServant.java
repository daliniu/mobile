package com.qq.taf.server;

import java.util.HashMap;
import java.util.Map;

import com.qq.jce.wup.UniAttribute;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

/**
 * 管理Servant
 * @author fanzhang
 */
public abstract class AdminServant extends Servant {
	
	private Map<String, ResponseHandler> responseHandlerMap=new HashMap<String, ResponseHandler>();
	
	public AdminServant() {
		responseHandlerMap.put("notify", new ResponseHandler() {
			public JceOutputStream handle(JceInputStream jis, JceCurrent jc) {
				return response_notify(jis, jc);
			}
			public UniAttribute handle_wup(UniAttribute unaIn, JceCurrent jc) {
				return response_notify_wup(unaIn, jc);
			}
		});
		responseHandlerMap.put("shutdown", new ResponseHandler() {
			public JceOutputStream handle(JceInputStream jis, JceCurrent jc) {
				return response_shutdown(jis, jc);
			}
			public UniAttribute handle_wup(UniAttribute unaIn, JceCurrent jc) {
				return response_shutdown_wup(unaIn, jc);
			}
		});
	}
	
	private JceOutputStream response_notify(JceInputStream jis,JceCurrent jc) {
		JceOutputStream jos=null;
		String command=jis.read("", 1, true);
		String ret=notify(command, jc);
		if(jc.isResponse()) {
			jos=new JceOutputStream(0);
			jos.write(ret, 0);
		}
		return jos;
	}
	
	private UniAttribute response_notify_wup(UniAttribute unaIn,JceCurrent jc) {
		UniAttribute unaOut=null;
		String command=unaIn.get("command");
		String ret=notify(command,jc);
		if(jc.isResponse()) {
			unaOut=new UniAttribute();
			unaOut.put("", ret);
		}
		return unaOut;
	}

	protected void async_response_notify(JceCurrent jc,String ret) {
		if(isWupRequest(jc)) {
			async_response_notify_wup(jc, ret);
		} else {
			JceOutputStream jos=new JceOutputStream(0);
			jos.write(ret, 0);
			sendResponseMessage(jc, jos);
		}
	}
	
	private void async_response_notify_wup(JceCurrent jc,String ret) {
		UniAttribute unaOut=new UniAttribute();
		unaOut.put("",ret);
		sendResponseMessage_wup(jc, unaOut);
	}
	
	private JceOutputStream response_shutdown(JceInputStream jis,JceCurrent jc) {
		JceOutputStream jos=null;
		shutdown(jc);
		if(jc.isResponse()) {
			jos=new JceOutputStream(0);
			jos.write(0, 0);
		}
		return jos;
	}
	
	private UniAttribute response_shutdown_wup(UniAttribute unaIn,JceCurrent jc) {
		UniAttribute unaOut=null;
		shutdown(jc);
		if(jc.isResponse()) {
			unaOut=new UniAttribute();
			unaOut.put("",0);
		}
		return unaOut;
	}
	
	protected void async_response_shutdown(JceCurrent jc) {
		if(isWupRequest(jc)) {
			async_response_shutdown_wup(jc);
		} else {
			JceOutputStream jos=new JceOutputStream(0);
			jos.write(0, 0);
			sendResponseMessage(jc,jos);
		}
	}
	
	private void async_response_shutdown_wup(JceCurrent jc) {
		UniAttribute unaOut=new UniAttribute();
		unaOut.put("",0);
		sendResponseMessage_wup(jc, unaOut);
	}

	@Override
	protected JceOutputStream doResponse(String funcName, JceInputStream jis, JceCurrent jc) {
		return responseHandlerMap.get(funcName).handle(jis,jc);
	}
	
	@Override
	protected UniAttribute doResponse_wup(String funcName, UniAttribute unaIn, JceCurrent jc) {
		return responseHandlerMap.get(funcName).handle_wup(unaIn, jc);
	}
	
	public abstract String notify(String command,JceCurrent jc);
	
	public abstract void shutdown(JceCurrent jc);

}
