package com.qq.jce.wup;

import java.io.IOException;
import java.util.Hashtable;

import com.qq.taf.RequestPacket;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.proxy.utils.HexUtil;
import com.qq.taf.proxy.utils.WrapByteArrayOutputStream;


public class UniPacket extends UniAttribute {

	private RequestPacket _package = new RequestPacket();

	public final static int UniPacketHeadSize = 4 ;

	public UniPacket() {
		_package.iVersion = 2;
	}
	
	public void put(String name, Object t) throws IOException {
		if (name.startsWith(".")) {
			throw new IllegalArgumentException("put name can not startwith . ");
		}
		super.put(name, t);
	}

	/**
	 * 将put的对象进行编码
	 */
	public byte[] encode() throws IOException {
		if (_package.sServantName.equals("")) {
			throw new IllegalArgumentException("servantName can not is null");
		}
		if (_package.sFuncName.equals("")) {
			throw new IllegalArgumentException("funcName can not is null");
		}
		JceOutputStream _os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		_os.write(_data, 0);
		_package.sBuffer = _os.getFlushBytes();
		_os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		this.writeTo(_os);
		byte[] bodys = _os.getFlushBytes();
		int size = bodys.length;
		WrapByteArrayOutputStream out = new WrapByteArrayOutputStream(UniPacketHeadSize + size);
		try {
			out.write(HexUtil.intToByte(size+UniPacketHeadSize));
			out.write(bodys);
		} catch (IOException ex) {
			ex.printStackTrace();
		}
		byte[] b = out.toByteArray();
		//System.out.println(WupHexUtil.bytes2HexStr(b));
		return b;
	}

	/**
	 * 对传入的数据进行解码 填充可get的对象
	 */
	public void decode(byte[] buffer) throws IOException {
		if (buffer.length < 4) {
			throw new IllegalArgumentException("decode package must include size head");
		}
		byte[] _newBuffer = new byte[buffer.length - UniPacketHeadSize];
		System.arraycopy(buffer, UniPacketHeadSize, _newBuffer, 0, _newBuffer.length);
		JceInputStream _is = new JceInputStream(_newBuffer);
		_is.setServerEncoding(encodeName);
		this.readFrom(_is);
		_is = new JceInputStream(_package.sBuffer);
		_is.setServerEncoding(encodeName);
		Hashtable _tempdata = new Hashtable();
		Hashtable h = new Hashtable();
		h.put("", new byte[0]);
		_tempdata.put("", h);
		_data = _is.readMap(_tempdata, 0, false);
	}

	/**
	 * 获取请求的service名字
	 * 
	 * @return
	 */
	public String getServantName() {
		return _package.sServantName;
	}

	/**
	 * 设置请求的service名字
	 * 
	 * @param servantName
	 */
	public void setServantName(String servantName) {
		_package.sServantName = servantName;
	}

	/**
	 * 获取请求的函数名字
	 * 
	 * @return
	 */
	public String getFuncName() {
		return _package.sFuncName;
	}

	/**
	 * 设置请求的函数名字
	 * 
	 * @param sFuncName
	 */
	public void setFuncName(String sFuncName) {
		_package.sFuncName = sFuncName;
	}

	/**
	 * 获取消息序列号
	 * 
	 * @return
	 */
	public int getRequestId() {
		return _package.iRequestId;
	}

	/**
	 * 设置消息序列号
	 * 
	 * @param iRequestId
	 */
	public void setRequestId(int iRequestId) {
		_package.iRequestId = iRequestId;
	}

	public void writeTo(JceOutputStream _os) throws IOException {
		_package.writeTo(_os);
	}

	public void readFrom(JceInputStream _is) throws IOException {
		_package.readFrom(_is);
	}

	public void display(java.lang.StringBuffer _os, int _level) {
		_package.display(_os, _level);
	}

	/**
	 * 通过请求包生成回应包，生成过程会从请求包获取请求ID、对象名称、方法名回填到回应包中， 同时和request的encodeName保持一致
	 * 
	 * @return
	 */
	public UniPacket createResponse() {
		UniPacket packet = new UniPacket();
		packet.setRequestId(getRequestId());
		packet.setServantName(getServantName());
		packet.setFuncName(getFuncName());
		packet.setEncodeName(encodeName);
		return packet;
	}
}
