package com.duowan.jce.wup;

import java.nio.ByteBuffer;
import java.util.HashMap;

import com.duowan.taf.RequestPacket;
import com.duowan.taf.cnst.Const;
import com.duowan.taf.jce.JceInputStream;
import com.duowan.taf.jce.JceOutputStream;
import com.duowan.taf.jce.JceUtil;

public class UniPacket extends UniAttribute {

	protected RequestPacket _package = new RequestPacket();

	public final static int UniPacketHeadSize = 4;

	public UniPacket() {
		_package.iVersion = Const.VERSION_WUP;
	}
	
	public UniPacket(boolean useVersion3) {
		if (useVersion3) {
			useVersion3();
		} else {
			_package.iVersion = Const.VERSION_WUP;
		}
		
	}
	
	/**
	 * 返回WUP编码版本
	 * @return
	 */
	public int getPackageVersion() {
		return _package.iVersion;
	}

	public <T> void put(String name, T t) {
		if (name.startsWith(".")) {
			throw new IllegalArgumentException("put name can not startwith . , now is "+name);
		}
		super.put(name, t);
	}
	
	/**
	 * 使用新版本编码
	 */
	public void useVersion3() {
		super.useVersion3();
		_package.iVersion = Const.VERSION_WUP3;
	}

	/**
	 * 将put的对象进行编码
	 */
	public byte[] encode() {
		if (_package.iVersion == Const.VERSION_WUP) {
			if (_package.sServantName == null || _package.sServantName.equals("")) {
				throw new IllegalArgumentException("servantName can not is null");
			}
			if (_package.sFuncName == null || _package.sFuncName.equals("")) {
				throw new IllegalArgumentException("funcName can not is null");
			}
		} else {
			if (_package.sServantName == null ) {
				_package.sServantName = "";
			}
			if (_package.sFuncName == null ) {
				_package.sFuncName = "";
			}
		}
		JceOutputStream _os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		if (_package.iVersion == Const.VERSION_WUP) {
			_os.write(_data, 0);
		} else {
			_os.write(_newData, 0);
		}
		_package.sBuffer = JceUtil.getJceBufArray(_os.getByteBuffer());
		_os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		this.writeTo(_os);
		byte[] bodys = JceUtil.getJceBufArray(_os.getByteBuffer());
		int size = bodys.length;
		ByteBuffer buf = ByteBuffer.allocate(size + UniPacketHeadSize);
		buf.putInt(size + UniPacketHeadSize).put(bodys).flip();
		return buf.array();
	}

	static HashMap<String, byte[]> newCache__tempdata = null;
	static HashMap<String, HashMap<String, byte[]>> cache__tempdata = null;
	
	/**
	 * 明确知道是version3的编码
	 * @param buffer
	 */
	public void decodeVersion3(byte[] buffer) {
		if (buffer.length < UniPacketHeadSize) {
			throw new IllegalArgumentException("decode package must include size head");
		}
		//暂不解析头 现在只有长度信息
        try {
        	JceInputStream _is = new JceInputStream(buffer,UniPacketHeadSize);
    		_is.setServerEncoding(encodeName);
    		this.readFrom(_is);
			_is = new JceInputStream(_package.sBuffer);
    		_is.setServerEncoding(encodeName);
    		if (newCache__tempdata == null) {
    			newCache__tempdata = new HashMap<String, byte[]>();
    			newCache__tempdata.put("", new byte[0]);
    		}
    		_newData = (HashMap<String, byte[]>) _is.readMap(newCache__tempdata, 0, false);
    		
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	
	/**
	 * 明确知道是Version2的版本
	 * @param buffer
	 */
	public void decodeVersion2(byte[] buffer) {
		if (buffer.length < UniPacketHeadSize) {
			throw new IllegalArgumentException("decode package must include size head");
		}
		//暂不解析头 现在只有长度信息
        try {
        	JceInputStream _is = new JceInputStream(buffer,UniPacketHeadSize);
    		_is.setServerEncoding(encodeName);
    		this.readFrom(_is);
			_is = new JceInputStream(_package.sBuffer);
    		_is.setServerEncoding(encodeName);
			if (cache__tempdata == null) {
    			cache__tempdata = new HashMap<String, HashMap<String, byte[]>>();
    			HashMap<String, byte[]> h = new HashMap<String, byte[]>();
    			h.put("", new byte[0]);
    			cache__tempdata.put("", h);
    		}
    		_data = (HashMap<String, HashMap<String, byte[]>>) _is.readMap(cache__tempdata, 0, false);
    		cachedClassName = new HashMap<String,Object>();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	
	/**
	 * 对传入的数据进行解码 填充可get的对象
	 */
	public void decode(byte[] buffer) {
		if (buffer.length < UniPacketHeadSize) {
			throw new IllegalArgumentException("decode package must include size head");
		}
		//暂不解析头 现在只有长度信息
        try {
        	JceInputStream _is = new JceInputStream(buffer,UniPacketHeadSize);
    		_is.setServerEncoding(encodeName);
    		this.readFrom(_is);
    		if (this._package.iVersion == Const.VERSION_WUP3 ) {
    			_is = new JceInputStream(_package.sBuffer);
        		_is.setServerEncoding(encodeName);
        		if (newCache__tempdata == null) {
        			newCache__tempdata = new HashMap<String, byte[]>();
        			newCache__tempdata.put("", new byte[0]);
        		}
        		_newData = (HashMap<String, byte[]>) _is.readMap(newCache__tempdata, 0, false);
    		} else {
    			_newData = null;
    			_is = new JceInputStream(_package.sBuffer);
        		_is.setServerEncoding(encodeName);
    			if (cache__tempdata == null) {
        			cache__tempdata = new HashMap<String, HashMap<String, byte[]>>();
        			HashMap<String, byte[]> h = new HashMap<String, byte[]>();
        			h.put("", new byte[0]);
        			cache__tempdata.put("", h);
        		}
        		_data = (HashMap<String, HashMap<String, byte[]>>) _is.readMap(cache__tempdata, 0, false);
        		 cachedClassName = new HashMap<String,Object>();
    		}
		} catch (Exception e) {
			throw new RuntimeException(e);
		}

		
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

	public void writeTo(JceOutputStream _os) {
		_package.writeTo(_os);
	}

	public void readFrom(JceInputStream _is) {
		_package.readFrom(_is);
	}

	public void display(java.lang.StringBuilder _os, int _level) {
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
		packet._package.iVersion = _package.iVersion;
		return packet;
	}
	
	private int oldRespIret = 0 ;

	/**
	 * 为兼容最早发布的客户端版本解码使用 iret字段始终为0
	 * 
	 * @return
	 */
	public byte[] createOldRespEncode() {
		JceOutputStream _os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		_os.write(_data, 0);
		byte[] oldSBuffer = JceUtil.getJceBufArray(_os.getByteBuffer());
		_os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		_os.write(_package.iVersion, 1);
		_os.write(_package.cPacketType, 2);
		_os.write(_package.iRequestId, 3);
		_os.write(_package.iMessageType, 4);
		_os.write(oldRespIret, 5);
		_os.write(oldSBuffer, 6);
		_os.write(_package.status, 7);
		return JceUtil.getJceBufArray(_os.getByteBuffer());
	}

	/**
	 * 为兼容最早发布的客户端版本取iret
	 * @return
	 */
	public int getOldRespIret() {
		return oldRespIret;
	}

	/**
	 * 为兼容最早发布的客户端版本设iret
	 * @param oldRespIret
	 */
	public void setOldRespIret(int oldRespIret) {
		this.oldRespIret = oldRespIret;
	}
}
