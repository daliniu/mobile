package com.qq.taf.server;

import com.qq.jce.wup.UniAttribute;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

/**
 * Servant的响应处理器
 * @author fanzhang
 */
public interface ResponseHandler {

	public JceOutputStream handle(JceInputStream jis,JceCurrent jc);
	
	public UniAttribute handle_wup(UniAttribute unaIn,JceCurrent jc);
	
}
