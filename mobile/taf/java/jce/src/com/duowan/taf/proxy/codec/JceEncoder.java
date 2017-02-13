package com.duowan.taf.proxy.codec;

import com.duowan.netutil.nio.mina2.core.buffer.IoBuffer;
import com.duowan.netutil.nio.mina2.core.session.IoSession;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolEncoderAdapter;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolEncoderOutput;



public class JceEncoder extends ProtocolEncoderAdapter {

	public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception {
		JceMessage request = (JceMessage) message;
		IoBuffer AllBuffer = IoBuffer.allocate(request.getPackageSize());
		AllBuffer.put(request.getBytes()).flip();
		out.write(AllBuffer);

		
	}

}