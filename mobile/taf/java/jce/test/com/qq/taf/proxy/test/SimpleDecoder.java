package com.qq.taf.proxy.test;

import java.util.List;

import com.qq.netutil.nio.LoggerCenter;
import com.qq.netutil.nio.mina2.fliter.codec.*;
import com.qq.netutil.nio.mina2.fliter.codec.statemachine.*;
import com.qq.taf.proxy.codec.JceDecodingStateMachine;


public class SimpleDecoder extends DecodingStateProtocolDecoder {

	public SimpleDecoder() {
		super(new SimpleDecoderStateMachine() {
			@Override
			protected DecodingState finishDecode(List<Object> childProducts, ProtocolDecoderOutput out) throws Exception {
				for (Object m : childProducts) {
					out.write(m);
				}
				return null;
			}
		});
	}

}
