package com.qq.taf.proxy.test;

import com.qq.jutil.crypto.HexUtil;
import com.qq.taf.ResponsePacket;
import com.qq.taf.jce.JceInputStream;
public class TestJce {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		byte[] allPackageBytes = HexUtil.hexStr2Bytes("000001090000010510012C");
		ResponsePacket response = new ResponsePacket();
		JceInputStream _is = new JceInputStream(allPackageBytes);
		//System.out.println(HexUtil.bytes2HexStr(allPackageBytes));
		response.readFrom(_is);


	}

}
