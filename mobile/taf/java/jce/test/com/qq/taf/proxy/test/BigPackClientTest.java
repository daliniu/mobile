package com.qq.taf.proxy.test;

import java.net.InetSocketAddress;

import com.qq.netutil.nio.mina2.core.filterchain.DefaultIoFilterChainBuilder;
import com.qq.netutil.nio.mina2.core.future.ConnectFuture;
import com.qq.netutil.nio.mina2.core.service.IoHandlerAdapter;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolCodecFilter;
import com.qq.netutil.nio.mina2.transport.socket.SocketSessionConfig;
import com.qq.netutil.nio.mina2.transport.socket.nio.NioSocketConnector;

public class BigPackClientTest extends IoHandlerAdapter{

	private ProtocolCodecFilter protocolCodec = new ProtocolCodecFilter(new SimpleCodecFactory());
	IoSession session ;
	
	static String remoteHost = "127.0.0.1";
	static int port = 9000;
	static String testData = "000000A710012C300140165627515150494D2E53657373696F6E5365727665722E53657373696F6E496E746572666163654F626A660C676574446174614974656D737D00005A162338323764353838643734326637363435386634623161343633623737373964343230322900050A0604636F6E7410FF0B0A0604636F6E7410FE0B0A0604636F6E7410FD0B0A0604636F6E7410FC0B0A0604636F6E7410FB0B810BB8980CA80C";
	static byte[] testBytes = HexUtils.convert(testData);
	static int readBufferSize = 1024;
	static int writeBufferSize = 1024;
	
	public void init() {
		NioSocketConnector socketConnector = new NioSocketConnector();
		DefaultIoFilterChainBuilder socketChain = socketConnector.getFilterChain();
		socketConnector.getSessionConfig().setKeepAlive(false);
		//socketConnector.getSessionConfig().setReadBufferSize(1024);
		//socketConnector.getSessionConfig().setReceiveBufferSize(1024);
		socketChain.addLast("codec", protocolCodec);
		socketConnector.setHandler(this);
		ConnectFuture future = socketConnector.connect(new InetSocketAddress(remoteHost, port));
		session = future.getSession();
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if ( args.length > 0 ) {
			remoteHost = args[0];
		}
		if ( args.length > 1 ) {
			port = Integer.parseInt(args[1]);
		}
		if ( args.length > 2 ) {
			readBufferSize = Integer.parseInt(args[2]);
		}
		if ( args.length > 3 ) {
			writeBufferSize = Integer.parseInt(args[3]);
		}
		System.out.println("remoteHost"+remoteHost+":"+port+" readBufferSize:"+readBufferSize+" writeBufferSize:"+writeBufferSize);
		BigPackClientTest t = new BigPackClientTest();
		t.init();
	}

	@Override
	public void messageReceived(IoSession session, Object message) throws Exception {
		System.out.println("received" + ((byte[])message).length );
	}
	
	public void exceptionCaught(IoSession session, Throwable cause) throws Exception {
		cause.printStackTrace();
	}
	
	public void sessionOpened(IoSession session) throws Exception {
		((SocketSessionConfig)session.getConfig()).setReceiveBufferSize(readBufferSize);
		((SocketSessionConfig)session.getConfig()).setSendBufferSize(writeBufferSize);
	}
	
}
