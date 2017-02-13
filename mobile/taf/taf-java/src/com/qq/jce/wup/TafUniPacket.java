package com.qq.jce.wup;

import java.util.HashMap;
import java.util.Map;

import com.qq.taf.cnst.Const;

/**
 * 通过WUP调用TAF需要使用的包类型
 * @author fanzhang
 */
public class TafUniPacket extends UniPacket {
	
	private static final long serialVersionUID = 1L;
	
	public TafUniPacket() {
		_package.iVersion=Const.VERSION_WUP;
		_package.cPacketType=0;
		_package.iMessageType=(int)0;
		_package.iTimeout=(int)0;
		_package.sBuffer=new byte[]{};
		_package.context=new HashMap<String, String>();
		_package.status=new HashMap<String, String>();
	}
	
    /**
     * 设置协议版本
     */	
    public void setTafVersion(short version) {
    	_package.iVersion=version;
    	if(version==Const.VERSION_WUP3) useVersion3();
    }

    /**
     * 设置调用类型
     */
    public void setTafPacketType(byte packetType) {
    	_package.cPacketType=packetType;
    }

    /**
     * 设置消息类型
     */
    public void setTafMessageType(int messageType) {
    	_package.iMessageType=messageType;
    }

    /**
     * 设置超时时间
     */
    public void setTafTimeout(int timeout) {
    	_package.iTimeout=timeout;
    }

    /**
     * 设置参数编码内容
     */
    public void setTafBuffer(byte[] buffer) {
    	_package.sBuffer=buffer;
    }

    /**
     * 设置上下文
     */
    public void setTafContext(Map<String,String> context) {
    	_package.context=context;
    }

    /**
     * 设置特殊消息的状态值
     */
    public void setTafStatus(Map<String,String> status) {
    	_package.status=status;
    }

    /**
     * 获取协议版本
     */
    public short getTafVersion() {
    	return _package.iVersion;
    }

    /**
     * 获取调用类型
     */
    public byte getTafPacketType() {
    	return _package.cPacketType;
    }

    /**
     * 获取消息类型
     */
    public int getTafMessageType() {
    	return _package.iMessageType;
    }

    /**
     * 获取超时时间
     */
    public int getTafTimeout() {
    	return _package.iTimeout;
    }

    /**
     * 获取参数编码后内容
     */
    public byte[] getTafBuffer() {
    	return _package.sBuffer;
    }

    /**
     * 获取上下文信息
     */
    public Map<String,String> getTafContext() {
    	return _package.context;
    }

    /**
     * 获取特殊消息的状态值
     */
    public Map<String,String> getTafStatus() {
    	return _package.status;
    }

    /**
     * 获取调用taf的返回值
     */
    public int getTafResultCode() {
    	String rcode=_package.status.get(Const.STATUS_RESULT_CODE);
    	int result=rcode!=null?Integer.parseInt(rcode):0;
    	return result;
    }

    /**
     * 获取调用taf的返回描述
     */
    public String getTafResultDesc() {
    	String rdesc=_package.status.get(Const.STATUS_RESULT_DESC);
    	String result=rdesc!=null?rdesc:"";
    	return result;
    }
    
}
