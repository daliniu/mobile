package com.duowan.taf.proxy.codec;

import com.duowan.netutil.nio.mina2.core.buffer.IoBuffer;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolDecoderOutput;
import com.duowan.netutil.nio.mina2.fliter.codec.statemachine.DecodingState;
import com.duowan.netutil.nio.mina2.fliter.codec.statemachine.DecodingStateMachine;
import com.duowan.netutil.nio.mina2.fliter.codec.statemachine.FixedLengthDecodingState;

public abstract class JceDecodingStateMachine extends DecodingStateMachine {

	boolean isRequest ;
	
	public JceDecodingStateMachine(boolean isRequest) {
		this.isRequest = isRequest;
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
//			try {
				packageLen = product.getInt();
				//LoggerCenter.info("read packageLen:"+packageLen);
//			} catch (Exception e) {
//				LoggerCenter.info(" finishDecode error , buffer:"+HexUtil.bytes2HexStr(product.array()));
//				throw e;
//			}
			if(packageLen>10*1024*1024 || packageLen<=0) {
				product.position(product.limit());
				throw new Exception("the length header of the request package must be between 0~10M bytes");
			}
			return new FixedLengthDecodingState(packageLen - JceMessage.HEAD_SIZE) {
				@Override
				protected DecodingState finishDecode(IoBuffer product, ProtocolDecoderOutput out) throws Exception {
					int size = packageLen- JceMessage.HEAD_SIZE;
					int bodySize = product.capacity();
					//LoggerCenter.info("read BodySize:"+bodySize);
					if ( bodySize < size ) {
						return null ;
					}
					byte[] reads = new byte[size];
					product.get(reads);
					JceMessage msg;
					if ( isRequest ) {
						//收到的信息是response
						msg = new JceMessage(false,reads);
					} else {
						msg = new JceMessage(true,reads);
					}
					out.write(msg);
					return null;
				}
			};
		}

	};



}
