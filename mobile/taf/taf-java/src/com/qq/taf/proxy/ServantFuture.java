package com.qq.taf.proxy;

import java.util.concurrent.CancellationException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

import com.qq.sim.Millis100TimeProvider;
import com.qq.taf.proxy.exec.TafCallTimeoutException;
import com.qq.taf.proxy.exec.TafException;

/**
 * 异步发送等待标示
 * @author albertzhu
 *
 */
public class ServantFuture {

	final int seq;
	boolean async;
	String sServantName;
	String sFuncName;
	long allowWaiteTimeMill;
	CallbackHandler handler;
	ServantProxyCallback servantProxyCallback;
	String remoteHost ;
	int remotePort;
	String endPointKey ;
	
	long bindSessionId;
	
	long startTime ;
	long costTime;
	private boolean isCancelled;
	private TafException failure;

	protected Object result;
	private boolean hasResult ;

	protected CountDownLatch latch = new CountDownLatch(1);
	
	public ServantFuture(int seq , long allowWaiteTimeMill) {
		this.seq = seq;
		startTime = Millis100TimeProvider.INSTANCE.currentTimeMillis();
		this.allowWaiteTimeMill = allowWaiteTimeMill;
	}

	public Object getResult() {
		return result;
	}

	public void setResult(Object result) {
		this.result = result;
		hasResult = true;
		notifyHaveResult();
	}

	public boolean cancel(boolean mayInterruptIfRunning) {
		isCancelled = true;
		notifyHaveResult();
		return true;
	}

	public boolean isCancelled() {
		return isCancelled;
	}

	public boolean isDone() {
		return latch.getCount() <= 0;
	}
	
	public void onNetCallFinished() {
		costTime = Millis100TimeProvider.INSTANCE.currentTimeMillis() - startTime;
	}

	public Object get() throws InterruptedException, ExecutionException {
		latch.await(allowWaiteTimeMill, TimeUnit.MILLISECONDS);
		if (isCancelled) {
			throw new CancellationException();
		} else if (failure != null) {
			throw failure ;
		} else if (hasResult) {
			return result;
		} else {
			throw new TafCallTimeoutException(sServantName+"."+sFuncName+" timout:"+allowWaiteTimeMill+" seq:"+seq);
		}
	}

	public void setFailure(TafException failure) {
		this.failure = failure;
		notifyHaveResult();
	}

	protected void notifyHaveResult() {
		latch.countDown();
	}

	public CallbackHandler getHandler() {
		return handler;
	}

	public void setHandler(CallbackHandler handler) {
		this.handler = handler;
	}

	public String getSFuncName() {
		return sFuncName;
	}

	public void setSFuncName(String sFuncName) {
		this.sFuncName = sFuncName;
	}

	public long getAllowWaiteTimeMill() {
		return allowWaiteTimeMill;
	}

	public void setAllowWaiteTimeMill(long allowWaiteTimeMill) {
		this.allowWaiteTimeMill = allowWaiteTimeMill;
	}
	
	public String getSServantName() {
		return sServantName;
	}

	public void setSServantName(String servantName) {
		sServantName = servantName;
	}

	public boolean isAsync() {
		return async;
	}

	public void setAsync(boolean async) {
		this.async = async;
	}

	public int getSeq() {
		return seq;
	}

	public long getStartTime() {
		return startTime;
	}

	public long getCostTime() {
		return costTime;
	}

	public Throwable getFailure() {
		return failure;
	}

	public long getBindSessionId() {
		return bindSessionId;
	}

	public void setBindSessionId(long bindSessionId) {
		this.bindSessionId = bindSessionId;
	}

	public String getRemoteHost() {
		return remoteHost;
	}

	public void setRemoteHost(String remoteHost) {
		this.remoteHost = remoteHost;
	}

	public int getRemotePort() {
		return remotePort;
	}

	public void setRemotePort(int remotePort) {
		this.remotePort = remotePort;
	}

	public String getEndPointKey() {
		return endPointKey;
	}

	public void setEndPointKey(String endPointKey) {
		this.endPointKey = endPointKey;
	}

	public ServantProxyCallback getServantProxyCallback() {
		return servantProxyCallback;
	}

	public void setServantProxyCallback(ServantProxyCallback servantProxyCallback) {
		this.servantProxyCallback = servantProxyCallback;
	}

}
