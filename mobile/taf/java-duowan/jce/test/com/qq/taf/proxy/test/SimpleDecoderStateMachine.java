package com.qq.taf.proxy.test;

import com.qq.jutil.crypto.HexUtil;
import com.qq.netutil.nio.LoggerCenter;
import com.qq.netutil.nio.mina2.core.buffer.*;
import com.qq.netutil.nio.mina2.fliter.codec.*;
import com.qq.netutil.nio.mina2.fliter.codec.statemachine.*;
import com.qq.taf.proxy.codec.JceMessage;

public abstract class SimpleDecoderStateMachine extends DecodingStateMachine {

	
	public SimpleDecoderStateMachine() {
	}
	
	//JceMessage msg;

	int packageLen = 0 ;
	
	@Override
	protected void destroy() throws Exception {
//		msg = null;
	}

//	@Override
//	protected DecodingState finishDecode(List<Object> childProducts, ProtocolDecoderOutput out) throws Exception {
//		return null;
//	}

	@Override
	protected DecodingState init() throws Exception {
//		msg = new JceMessage();
		return READ_PACKAGES_LENS;
	}

	/**
	 * 读取整个包的长度
	 * 远程主动关闭连接的时候 如果残余的buffer 判断长度并尝试解析
	 */
	private final DecodingState READ_PACKAGES_LENS = new FixedLengthDecodingState(JceMessage.HEAD_SIZE) {
		@Override
		protected DecodingState finishDecode(IoBuffer product, ProtocolDecoderOutput out) throws Exception {
			if ( product.capacity() < 4 ) {
				return null ;
			}
			try {
				packageLen = product.getInt();
				LoggerCenter.info("read packageLen:"+packageLen);
			} catch (Exception e) {
				LoggerCenter.info(" finishDecode error , buffer:"+HexUtil.bytes2HexStr(product.array()));
				throw e;
			}
			return new FixedLengthDecodingState(packageLen - JceMessage.HEAD_SIZE) {
				@Override
				protected DecodingState finishDecode(IoBuffer product, ProtocolDecoderOutput out) throws Exception {
					int size = packageLen- JceMessage.HEAD_SIZE;
					int bodySize = product.capacity();
					LoggerCenter.info("read BodySize:"+bodySize);
					if ( bodySize < size ) {
						return null ;
					}
					byte[] reads = new byte[size];
					product.get(reads);
					out.write(reads);
					return null;
				}
			};
		}

	};



}
