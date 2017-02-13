package com.qq.taf.server;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.HashMap;
import java.util.Map;

import com.qq.jce.wup.UniAttribute;
import com.qq.jutil.string.StringUtil;
import com.qq.netutil.nio.mina2.core.service.IoAcceptor;
import com.qq.netutil.nio.mina2.core.service.IoHandlerAdapter;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.transport.socket.DatagramAcceptor;
import com.qq.netutil.nio.mina2.transport.socket.SocketAcceptor;
import com.qq.taf.RequestPacket;
import com.qq.taf.ResponsePacket;
import com.qq.taf.cnst.Const;
import com.qq.taf.cnst.JCEMESSAGETYPEGRID;
import com.qq.taf.cnst.JCEMESSAGETYPESAMPLE;
import com.qq.taf.cnst.JCESERVERSUCCESS;
import com.qq.taf.cnst.JCESERVERUNKNOWNERR;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.SendMessage;
import com.qq.taf.proxy.SendMsgStatBody;
import com.qq.taf.proxy.ServantProxyThreadData;
import com.qq.taf.proxy.ServantProxyThreadData.SampleKey;
import com.qq.taf.proxy.TimerProvider;
import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.server.util.GeneralException;

/**
 * 对象基类
 * @author fanzhang
 */
public abstract class Servant extends IoHandlerAdapter {
	
	private AdapterConfig adapConf;
	private IoAcceptor acceptor;
	private ManagedExecutor executor;
	
	private volatile Object proxy;
	
	private class MessageReceivedTask implements Runnable {
		private IoSession session;
		private Object message;
		public MessageReceivedTask(IoSession session,Object message) {
			this.session=session;
			this.message=message;
		}
		public void run() {
			handle((JceMessage)message, session);
		}
		
		public boolean isNeedStat() {
			RequestPacket request = ((JceMessage)message).getRequest();
			if(request == null) return false;
			if(request.iVersion==Const.VERSION_WUP || request.iVersion==Const.VERSION_WUP3 ||request.cPacketType == Const.PACKET_TYPE_JCEONEWAY) {
				return true;
			}
			return false;
		}
		
		public Boolean get() {
			return isNeedStat();
		}
	}
	
	
	public Object getProxy() {
		if(proxy != null) {
			return proxy;
		}
		return this;
	}
	
	public void setProxy(Object proxy) {
		this.proxy = proxy;
	}

	public Servant() {
		initialize();
	}
	
	public void setAdapterConfig(AdapterConfig adapConf) {
		this.adapConf=adapConf;
	}
	
	public void initAcceptor(SocketAcceptor socketAcceptor,DatagramAcceptor datagramAcceptor) {
		this.acceptor=adapConf.getEndpoint().getProtocol().equals("tcp")?socketAcceptor:datagramAcceptor;
	}
	
	public void initExecutor() {
		executor=new ManagedExecutor(adapConf.getServant(),adapConf.getQueueCapacity(),adapConf.getQueueTimeout(),adapConf.getThreads());
	}
	
	public void bind() throws Throwable {
		acceptor.bind(getBindAddress());
	}
	
	public void unbind() throws Throwable {
		acceptor.unbind(getBindAddress());
	}
	
	public int getQueueSize() {
		return executor.getQueueSize();
	}
	
	public int getSessionCount() {
		try {
			return SessionCheckerFilter.getInstance().getSessionCount(getBindAddress());
		} catch(Throwable t) {
			return 0;
		}
	}
	
	@Override
	public void messageReceived(IoSession session, Object message) throws Exception {
		executor.execute(new MessageReceivedTask(session,message));
	}
	
	@Override
	public void exceptionCaught(IoSession session, Throwable cause) throws Exception {
		SystemLogger.record("ErrorFound|"+adapConf.getServant()+"|"+session.getLocalAddress().toString()+"|"+session.getRemoteAddress().toString(),cause);
		session.close(true);
	}
	
	public InetSocketAddress getBindAddress() throws Throwable {
		return new InetSocketAddress(InetAddress.getByName(adapConf.getEndpoint().getHost()),adapConf.getEndpoint().getPort());
	}
	
	private void handle(JceMessage reqMsg,IoSession session) {
		long begin = TimerProvider.getNow();
		boolean hasExec = false;
		JceCurrent jc = null;
		try {
			jc=new JceCurrent();
			jc.setIoSession(session);
			jc.setRequestPacket(reqMsg.getRequest());
			jc.setResponseRet(JCESERVERSUCCESS.value);
			if(processGrid(jc)) return;
			
			processDyeing(jc);
			
			saveSampleKey(jc);
			
			if(isPingRequest(jc)) response_ping(jc); 
			else if(isWupRequest(jc)) response_wup(jc); 
			else response(jc);
		}
		catch(Throwable th) {
			hasExec = true;
			GeneralException.raise(th);
		}
		finally {
			if(isNeedServerStat(jc)) {
				try {
					SocketAddress addr = session.getRemoteAddress();
					ServerStatTool.Instance.addStat("OneWayStat/WupStat", reqMsg.getRequest().sServantName, reqMsg.getRequest().sFuncName, ((InetSocketAddress)addr).getAddress().getHostAddress(), adapConf.getEndpoint().getHost(), adapConf.getEndpoint().getPort(), jc.getResponseRet(),TimerProvider.getNow()-begin, hasExec?SendMsgStatBody.CallStatusExec:SendMsgStatBody.CallStatusSucc);
				}
				catch(Throwable th) {
					SystemLogger.record("find error in stat report", th);
				}
			}
		}
	}
	
	private boolean isNeedServerStat(JceCurrent jc ) {
		return isWupRequest(jc)||this.isOneWayRequest(jc);
	}
	
	private boolean isPingRequest(JceCurrent jc) {
		return jc.getRequestPacket().sFuncName.equals("taf_ping");
	}
	
	private boolean processGrid(JceCurrent jc) {
		int clientGrid=-1;
		String clientKey="";
		int serverGrid=adapConf.getEndpoint().getGrid();
		boolean isGridMessage=SendMessage.isMsgType(jc.getRequestPacket().iMessageType, JCEMESSAGETYPEGRID.value);
		
		if(isGridMessage) {
			Map<String, String> status=jc.getRequestPacket().status;
			String msgKey=status.get(Const.STATUS_GRID_KEY);
			String msgGrid=status.get(Const.STATUS_GRID_CODE);
			if(msgKey!=null && msgGrid!=null) {
				clientKey=msgKey;
				clientGrid=Integer.parseInt(msgGrid);
			}
			ServantProxyThreadData.INSTANCE.getData()._routeKey=clientKey;
			ServantProxyThreadData.INSTANCE.getData()._gridCode=clientGrid;
		}
		
		if(serverGrid!=0 && !isPingRequest(jc)) {
			if(!isGridMessage || clientGrid!=serverGrid) {
				Map<String,String> status=new HashMap<String, String>();
				status.put(Const.STATUS_GRID_CODE, serverGrid+"");
				if(isWupRequest(jc)) sendResponseMessageWithStatus_wup(jc, null, status); else sendResponseMessageWithStatus(jc, null, status);
				return true;
			}
		}
		
		return false;
	}
	
	private void processDyeing(JceCurrent jc) {
		String tdDyeingKey="";
		String dyeingKey=jc.getRequestPacket().status.get(Const.STATUS_DYED_KEY);
		if(dyeingKey!=null && dyeingKey.trim().length()!=0) {
			tdDyeingKey=dyeingKey;
		} else if(DyeingManager.getInstance().isDyeing()) {
			String gridKey=jc.getRequestPacket().status.get(Const.STATUS_GRID_KEY);
			if(gridKey!=null && gridKey.trim().length()!=0 
					&& DyeingManager.getInstance().isDyeingReq(gridKey, jc.getRequestPacket().sServantName, jc.getRequestPacket().sFuncName)) {
				tdDyeingKey=gridKey;
			}
		}
		ServantProxyThreadData.INSTANCE.getData()._dyeingKey=tdDyeingKey;
	}
	
	private void saveSampleKey(JceCurrent jc) {
		SampleKey sk=ServantProxyThreadData.INSTANCE.getData()._sampleKey;
		
		sk._root=true;
		sk._width=0;
		sk._depth=0;
		sk._unid="";
		sk._parentWidth=-1;
		
		if(SendMessage.isMsgType(jc.getRequestPacket().iMessageType, JCEMESSAGETYPESAMPLE.value)) {
			sk._root=false;
			String[] v=StringUtil.split(jc.getRequestPacket().status.get(Const.STATUS_SAMPLE_KEY), "|");
			if(v!=null && v.length>2) {
				sk._unid=v[0];
				sk._depth=Integer.parseInt(v[1])+1;
				sk._parentWidth=Integer.parseInt(v[2]);
			}
		}
	}
	
	private void response_ping(JceCurrent jc) {
		sendResponseMessage(jc,null);
		SystemLogger.record("Ping|ok|"+jc.getIoSession().getRemoteAddress().toString());
	}
	
	private void response(JceCurrent jc) {
		JceInputStream jis=new JceInputStream(jc.getRequestPacket().sBuffer);
		jis.setServerEncoding(getCharacterSet());
		JceOutputStream jos=null;
		
		try {
			jos=doResponse(jc.getRequestPacket().sFuncName,jis,jc);
		} catch(Throwable t) {
			jc.setResponseRet(JCESERVERUNKNOWNERR.value);
			sendResponseMessage(jc, null);
			GeneralException.raise(t);
		}
		//modify by easezhang for 单项请求不需要发送响应
		if(jc.isResponse() && !isOneWayRequest(jc)) 
			sendResponseMessage(jc, jos);
	}
	
	private boolean isOneWayRequest(JceCurrent jc) { 
		return jc.getRequestPacket().cPacketType == Const.PACKET_TYPE_JCEONEWAY;
	}
	private void response_wup(JceCurrent jc) {
		UniAttribute unaIn=new UniAttribute();
		unaIn.setEncodeName(getCharacterSet());
		if(isWup3(jc)) unaIn.decodeVersion3(jc.getRequestPacket().sBuffer); else unaIn.decodeVersion2(jc.getRequestPacket().sBuffer);
		UniAttribute unaOut=null;
		
		try {
			unaOut=doResponse_wup(jc.getRequestPacket().sFuncName, unaIn, jc);
		} catch(Throwable t) {
			jc.setResponseRet(JCESERVERUNKNOWNERR.value);
			sendResponseMessage_wup(jc, null);
			GeneralException.raise(t);
		}
		
		if(jc.isResponse()) sendResponseMessage_wup(jc, unaOut);
	}
	
	private void sendResponseMessageWithStatus(JceCurrent jc,JceOutputStream jos,Map<String,String> status) {
		ResponsePacket resPck=new ResponsePacket();
		
		resPck.cPacketType = jc.getRequestPacket().cPacketType;
		resPck.iRequestId = jc.getRequestPacket().iRequestId;
		resPck.iRet = jc.getResponseRet();
		resPck.iVersion = jc.getRequestPacket().iVersion;
		resPck.sBuffer=jos!=null?JceUtil.getJceBufArray(jos.getByteBuffer()):new byte[]{};
		if(status!=null) resPck.status=status;
		
		JceOutputStream josAll=new JceOutputStream(0);
		josAll.setServerEncoding(getCharacterSet());
		resPck.writeTo(josAll);
		
		JceMessage responseMsg = JceMessage.createJceMessage(false, resPck.iRequestId, JceUtil.getJceBufArray(josAll.getByteBuffer()));
		jc.getIoSession().write(responseMsg);	
	}
	
	protected void sendResponseMessage(JceCurrent jc,JceOutputStream jos) {
		sendResponseMessageWithStatus(jc, jos, null);
	}
	
	private void sendResponseMessageWithStatus_wup(JceCurrent jc,UniAttribute unaOut,Map<String,String> status) {
		RequestPacket unResPck=new RequestPacket();
		
		unResPck.iVersion=jc.getRequestPacket().iVersion;
		unResPck.cPacketType=jc.getRequestPacket().cPacketType;
		unResPck.iMessageType=jc.getRequestPacket().iMessageType;
		unResPck.iRequestId=jc.getRequestPacket().iRequestId;
		unResPck.sServantName=jc.getRequestPacket().sServantName;
		unResPck.sFuncName=jc.getRequestPacket().sFuncName;
		unResPck.sBuffer=unaOut!=null?unaOut.encode():new byte[]{};
		unResPck.iTimeout=jc.getRequestPacket().iTimeout;
		unResPck.context=jc.getRequestPacket().context;
		unResPck.status=jc.getRequestPacket().status;
		
		unResPck.status.put(Const.STATUS_RESULT_CODE,jc.getResponseRet()+"");
		unResPck.status.put(Const.STATUS_RESULT_DESC,jc.getResponseRet()==0?"SUCCESS":"UNKNOWN_EXCEPTION");
		if(status!=null) unResPck.status.putAll(status);
		
		JceOutputStream jos=new JceOutputStream();
		jos.setServerEncoding(getCharacterSet());
		unResPck.writeTo(jos);
		
		JceMessage unResMsg=JceMessage.createJceMessage(false, unResPck.iRequestId, JceUtil.getJceBufArray(jos.getByteBuffer()));
		jc.getIoSession().write(unResMsg);		
	}
	
	protected void sendResponseMessage_wup(JceCurrent jc,UniAttribute unaOut) {
		sendResponseMessageWithStatus_wup(jc, unaOut, null);
	}
	
	protected boolean isWupRequest(JceCurrent jc) {
		return jc.getRequestPacket().iVersion==Const.VERSION_WUP || jc.getRequestPacket().iVersion==Const.VERSION_WUP3;
	}
	
	protected boolean isWup3(JceCurrent jc) {
		return jc.getRequestPacket().iVersion==Const.VERSION_WUP3;
	}
	
	protected void initialize() {
	}
	
	protected String getCharacterSet() {
		return "GBK";
	}
	
	protected abstract JceOutputStream doResponse(String funcName,JceInputStream jis,JceCurrent jc);
	
	protected abstract UniAttribute doResponse_wup(String funcName,UniAttribute unaIn,JceCurrent jc);
	
}
