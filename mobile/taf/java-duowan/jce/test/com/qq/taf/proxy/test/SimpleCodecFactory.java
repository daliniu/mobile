package com.qq.taf.proxy.test;

import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolCodecFactory;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolDecoder;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolEncoder;

public class SimpleCodecFactory implements ProtocolCodecFactory {

	public ProtocolDecoder getDecoder(IoSession session) throws Exception {
		return new SimpleDecoder();
	}

	public ProtocolEncoder getEncoder(IoSession session) throws Exception {
		return new SimpleEncoder();
	}
	
}
