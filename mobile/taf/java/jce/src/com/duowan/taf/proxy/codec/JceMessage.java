package com.duowan.taf.proxy.codec;

import java.nio.ByteBuffer;

import com.duowan.jutil.crypto.HexUtil;
import com.duowan.taf.RequestPacket;
import com.duowan.taf.ResponsePacket;
import com.duowan.taf.jce.JceInputStream;
import com.duowan.taf.proxy.ServantFuture;

public class JceMessage {
	
	public final static int HEAD_SIZE = 4 ;
	
	int packageSize = 0 ;
	
	byte[] bodys ;
	
	int seq ;
	
	ServantFuture future;
	
	RequestPacket request;
	
	ResponsePacket response;
		
	public static int getHEAD_SIZE() {
		return HEAD_SIZE;
	}


	public RequestPacket getRequest() {
		return request;
	}

	public ResponsePacket getResponse() {
		return response;
	}

	public boolean isRequest() {
		return isRequest;
	}

	
	
	boolean isRequest;

	/**
	 * 从读取到的bytes中解析出JceMessage
	 * 已经去掉头部长度信息
	 * @param isRequest
	 * @param allPackageBytes
	 */
	public JceMessage( boolean isRequest , byte[] allPackageBytes) {
		if ( isRequest ) {
			request = new RequestPacket();
			JceInputStream _is = new JceInputStream(allPackageBytes);
			request.readFrom(_is);
			this.seq = request.iRequestId ;
			setBodys(allPackageBytes);
		} else {
			response = new ResponsePacket();
			JceInputStream _is = new JceInputStream(allPackageBytes);
			//System.out.println(HexUtil.bytes2HexStr(allPackageBytes));
			response.readFrom(_is);
			this.seq = response.iRequestId ;
			setBodys(allPackageBytes);
		}
	}
	
	public static JceMessage createJceMessage( boolean isRequest , int seq , byte[] bodyBytes ) {
		ByteBuffer allBuffer = ByteBuffer.allocate(4 + bodyBytes.length);
		allBuffer.putInt(allBuffer.capacity()).put(bodyBytes).flip();
		return new JceMessage(seq,allBuffer.array());
	}
	
	public static  JceMessage createErrorRespMessage( int _errorRet , int seq  ) {
		JceMessage msg =  new JceMessage(seq,new byte[]{});
		ResponsePacket _resp = new ResponsePacket();
		_resp.iRet = _errorRet;
		_resp.iRequestId = seq;
		msg.response = _resp;
		return msg;
	}
	
	private JceMessage(int seq, byte[] allBodys ) {
		this.seq = seq;
		setBodys(allBodys);
	}

	public int getPackageSize() {
		return packageSize;
	}
	
	public void setPackageSize(int packageSize) {
		this.packageSize = packageSize;
	}

	public byte[] getBytes() {
		return bodys;
	}

	public byte[] getBodys() {
		return bodys;
	}

	public void setBodys(byte[] bodys) {
		this.bodys = bodys;
		packageSize = bodys.length  ;
	}

	public int getSeq() {
		return seq;
	}

	public void setSeq(int seq) {
		this.seq = seq;
	}
	
	public String toString() {
		return "seq:"+seq+" "+HexUtil.bytes2HexStr(bodys);
	}


	public ServantFuture getFuture() {
		return future;
	}


	public void setFuture(ServantFuture future) {
		this.future = future;
	}

	
}
