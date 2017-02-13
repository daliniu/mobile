package com.duowan.taf.proxy;

import java.net.InetAddress;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import com.duowan.taf.RequestPacket;
import com.duowan.taf.StatSampleMsg;
import com.duowan.taf.cnst.Const;
import com.duowan.taf.cnst.JCEMESSAGETYPEGRID;
import com.duowan.taf.cnst.JCEMESSAGETYPEHASH;
import com.duowan.taf.cnst.JCEMESSAGETYPESAMPLE;
import com.duowan.taf.proxy.ServantProxyThreadData.SampleKey;
import com.duowan.taf.proxy.ServantProxyThreadData.ThreadPrivateData;

/**
 * 发送信息的封装
 * @author albertzhu
 *
 */
public class SendMessage implements InvokeInfo {

	String objectName;
	String methodName;
	byte[] sBuffer;
	Map<String, String> map;
	Map<String, String> status;
	ServantFuture future;
	boolean async;
	boolean needReturn;
	
	RequestPacket request = null;
	int iMessageType; 
	int iTimeout;
	ServantProxy proxy;
	public StatSampleMsg sample;
	
	int hash=Const.INVALID_HASH_CODE;
	int gridValue = Const.INVALID_GRID_CODE ;

	public SendMessage(String objectName, String methodName, byte[] sBuffer, int iTimeout, Map<String, String> map, Map<String, String> status,
			ServantFuture future, CallbackHandler handler, boolean needReturn) {
		this.objectName = objectName;
		this.methodName = methodName;
		this.sBuffer = sBuffer;
		this.map = map;
		this.status = status;
		this.future = future;
		this.async = future.isAsync();
		this.future.setSServantName(objectName);
		this.future.setSFuncName(methodName);
		this.future.setHandler(handler);
		this.needReturn = needReturn;
		this.iTimeout = iTimeout;
		if (needReturn) {
			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCENORMAL, iMessageType,future.getSeq(), objectName,
					methodName, sBuffer, iTimeout, map,status);
		} else {
			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCEONEWAY, iMessageType ,future.getSeq(), objectName,
					methodName, sBuffer, iTimeout, map,status);
		}
	}
	
	public SendMessage(String objectName, String methodName, byte[] sBuffer, int iTimeout, Map<String, String> map, Map<String, String> status,
			ServantFuture future, ServantProxyCallback handler, boolean needReturn) {
		this.objectName = objectName;
		this.methodName = methodName;
		this.sBuffer = sBuffer;
		this.map = map;
		this.status = status;
		this.future = future;
		this.async = future.isAsync();
		this.future.setSServantName(objectName);
		this.future.setSFuncName(methodName);
		this.future.setServantProxyCallback(handler);
		this.needReturn = needReturn;
		this.iTimeout = iTimeout;
		if (needReturn) {
			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCENORMAL, iMessageType,future.getSeq(), objectName,
					methodName, sBuffer, iTimeout, map,status);
		} else {
			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCEONEWAY, iMessageType ,future.getSeq(), objectName,
					methodName, sBuffer, iTimeout, map,status);
		}
	}

	public RequestPacket getRequestPacket() {
		return request;
	}

	public String getObjectName() {
		return objectName;
	}

	public String getMethodName() {
		return methodName;
	}

	public byte[] getSBuffer() {
		return sBuffer;
	}

	public Map<String, String> getMap() {
		return map;
	}

	public ServantFuture getFuture() {
		return future;
	}

	public boolean isNeedReturn() {
		return needReturn;
	}

	public int getInvokeHash() {
		return hash;
	}

	public boolean isAsync() {
		return async;
	}

	public void setAsync(boolean async) {
		this.async = async;
	}

	public boolean isMsgType(int messageType) {
		return (iMessageType&messageType) != 0 ;
	}

	public void setMessageType(int messageType) {
		iMessageType |= messageType ;
		request.iMessageType = iMessageType;
//		if (needReturn) {
//			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCENORMAL, iMessageType,future.getSeq(), objectName,
//					methodName, sBuffer, iTimeout, map,status);
//		} else {
//			request = new RequestPacket(ServantProxy.version, Const.PACKET_TYPE_JCEONEWAY, iMessageType ,future.getSeq(), objectName,
//					methodName, sBuffer, iTimeout, map,status);
//		}
	}

	public Map<String, String> getStatus() {
		return status;
	}

	public void setStatus(Map<String, String> status) {
		this.status = status;
	}

	public void setMap(Map<String, String> map) {
		this.map = map;
	}

	public ServantProxy getProxy() {
		return proxy;
	}

	public void setProxy(ServantProxy proxy) {
		this.proxy = proxy;
	}

	public int getInvokeGrid() {
		return gridValue;
	}
	
	private static final AtomicInteger NUM=new AtomicInteger(0); 
	
	private static String sampleUnid() {
		byte[] ip=null;
		try {
			ip=InetAddress.getByName(CommunicatorConfig.localIP).getAddress();
		} catch(Throwable t) {
			ip=new byte[]{127,0,0,1};
		}
		int time=(int)(TimerProvider.getNow()&0x00000000ffffffffL);
		int thread=(int)(Thread.currentThread().getId()&0x00000000ffffffffL);
		short num=(short)(NUM.getAndIncrement()&0x0000ffff);
		String unid=String.format("%02x%02x%02x%02x%08x%08x%04x",ip[0],ip[1],ip[2],ip[3],time,thread,num);
		return unid;
	}
	
	public void setSample() {
		String unid="";
		int depth=0;
		int width=0;
		int parentWidth=0;
		
		SampleKey sk=ServantProxyThreadData.INSTANCE.getData()._sampleKey;
		if(sk._root) {
			if(sk._parentWidth==-1) sk._root=false;
			unid=sampleUnid();
		} else {
			unid=sk._unid;
			depth=sk._depth;
			width=sk._width;
			parentWidth=sk._parentWidth;
		}
		
		if(unid.length()==0) return;
		
		sample=new StatSampleMsg();
	    sample.unid = unid; 
	    sample.depth = depth;
	    sample.width = width;   
	    sample.parentWidth = parentWidth;
	    sample.masterName = null;
	    sample.slaveName = objectName;
	    sample.interfaceName = methodName;
	    sample.masterIp = "";
	    sample.slaveIp = null;
		
		sk._unid=unid;
		sk._depth=depth;
		sk._width=width+1;
		sk._parentWidth=parentWidth;
	
		status.put(Const.STATUS_SAMPLE_KEY, unid+"|"+depth+"|"+width);		
		setMessageType(JCEMESSAGETYPESAMPLE.value);
	}
	
	public void setInvokeGrid() {
		if ( status.containsKey(Const.STATUS_GRID_KEY) ) {
			String routerKey = status.get(Const.STATUS_GRID_KEY);
			if(proxy.taf_Router()!=null) gridValue=proxy.taf_Router().getGridByKey(routerKey);
			if(gridValue==Const.INVALID_GRID_CODE) {
				ThreadPrivateData tpd=ServantProxyThreadData.INSTANCE.getData();
				if(routerKey.equals(tpd._routeKey) && tpd._gridCode!=Const.INVALID_GRID_CODE) gridValue=tpd._gridCode;
			}
			if ( gridValue != Const.INVALID_GRID_CODE) {
				status.put(Const.STATUS_GRID_CODE,String.valueOf(gridValue));
				setMessageType(JCEMESSAGETYPEGRID.value);
			}
		}
	}
	
	public void setInvokeHash() {
		hash=ServantProxyThreadData.INSTANCE.getData()._hashCode;
		if ( hash != Const.INVALID_HASH_CODE ) setMessageType(JCEMESSAGETYPEHASH.value);
	}
	
	public void setInvokeDyeing() {
		String dyeingKey=ServantProxyThreadData.INSTANCE.getData()._dyeingKey;
		if(dyeingKey!=null && dyeingKey.trim().length()!=0) status.put(Const.STATUS_DYED_KEY,dyeingKey);
	}
	
	public static boolean isMsgType(int type,int messageType) {
		return (type&messageType)!=0;
	}
	
	public static void main(String[] args) throws Throwable {
		for(int i=0;i<10;i++) {
			new Thread(new Runnable(){
				public void run() {
					System.out.println(sampleUnid());		
				}
			}).start();
			Thread.sleep(50);
		}
	}
}
