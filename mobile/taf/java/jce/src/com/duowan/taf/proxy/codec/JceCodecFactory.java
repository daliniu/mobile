package com.duowan.taf.proxy.codec;

import com.duowan.netutil.nio.mina2.core.session.IoSession;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolCodecFactory;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolDecoder;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolEncoder;



public class JceCodecFactory implements ProtocolCodecFactory {

	boolean isRequest ;
	public JceCodecFactory(boolean isRequest) {
		this.isRequest = isRequest ;
	}
	
	public ProtocolDecoder getDecoder(IoSession session) throws Exception {
		return new JceDecoder(isRequest);
	}

	public ProtocolEncoder getEncoder(IoSession session) throws Exception {
		return new JceEncoder();
	}

}
