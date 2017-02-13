package com.qq.taf.proxy.test;

import com.qq.jutil.crypto.HexUtil;
import com.qq.netutil.nio.mina2.core.buffer.IoBuffer;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolEncoderAdapter;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolEncoderOutput;
import com.qq.taf.proxy.codec.JceMessage;



public class SimpleEncoder extends ProtocolEncoderAdapter {

	public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception {
		byte[] request = (byte[]) message;
		IoBuffer AllBuffer = IoBuffer.allocate(request.length);
		AllBuffer.put(request).flip();
		out.write(AllBuffer);

		
	}

}