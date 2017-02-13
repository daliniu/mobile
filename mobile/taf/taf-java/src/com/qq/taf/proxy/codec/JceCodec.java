package com.qq.taf.proxy.codec;

import java.nio.ByteBuffer;

import com.qq.navi.Filter;
import com.qq.navi.FilterDecodeProduct;
import com.qq.navi.FilterEncodeProduct;
import com.qq.navi.Session;

/**
 * JCE编解码器
 * @author fanzhang
 */
public class JceCodec extends Filter {
	
	private boolean isClient;
	
	public JceCodec(boolean isClient) {
		this.isClient=isClient;
	}

	@Override
	public void decode(Session session, ByteBuffer buffer, FilterDecodeProduct decodeProduct) throws Throwable {
		ByteBuffer now=buffer;
		ByteBuffer prev=(ByteBuffer)session.getAttribute("codec_buff");
		if(prev!=null) {
			session.removeAttribute("codec_buff");
			now=(ByteBuffer)ByteBuffer.allocate(prev.remaining()+buffer.remaining()).put(prev).put(buffer).flip();
		}
		while(now.remaining()>4 && now.remaining()>=now.getInt(now.position())) {
			int len=now.getInt();
			if(len>10*1024*1024 || len<=0) {
				buffer.position(buffer.limit());
				now.position(now.limit());
				throw new Exception("the length header of the request package must be between 0~10M bytes");
			}
			byte[] data=new byte[len-4];
			now.get(data);
			JceMessage message=new JceMessage(!isClient,data);
			decodeProduct.add(session, message);
		} 
		if(now.hasRemaining()) {
			ByteBuffer succ=(ByteBuffer)ByteBuffer.allocate(now.remaining()).put(now).flip();
			session.setAttribute("codec_buff", succ);
		}		
	}
	
	@Override
	public void encode(Session session, Object message, FilterEncodeProduct encodeProduct) throws Throwable {
		JceMessage jceMessage=(JceMessage)message;
		ByteBuffer buffer=(ByteBuffer)ByteBuffer.allocate(jceMessage.getPackageSize()).put(jceMessage.getBytes()).flip();
		encodeProduct.add(session, buffer);
	}
	
}
