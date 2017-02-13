package com.duowan.taf.proxy.codec;

import java.util.List;

import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolDecoderOutput;
import com.duowan.netutil.nio.mina2.fliter.codec.statemachine.DecodingState;
import com.duowan.netutil.nio.mina2.fliter.codec.statemachine.DecodingStateProtocolDecoder;


public class JceDecoder extends DecodingStateProtocolDecoder {

	public JceDecoder(boolean isRequest) {
		super(new JceDecodingStateMachine(isRequest) {
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
