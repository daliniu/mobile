package com.qq.taf.server.ahttpclient;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.httpclient.client.ContentExchange;
import com.qq.httpclient.client.HttpClient;
import com.qq.httpclient.http.HttpVersions;
import com.qq.httpclient.util.thread.QueuedThreadPool;
import com.qq.jutil.util.TimeoutDetector;
import com.qq.jutil.util.TimeoutHandler;

public class AsyncHttpClient {
	
	private static HttpClient client = null;

	static Object lock = new Object();
	
	private static final String ATTR_REQUEST_INFO = "__ri__";
	
	static QueuedThreadPool pool;
	
	public AsyncHttpClient() {
		pool = new QueuedThreadPool();
		pool.setMaxThreads(128);
		pool.setDaemon(true);
		pool.setName("HttpClient");
		td = new TimeoutDetector<HttpRequestInfo>();
		td.startHandleThread(new RequestInfoTimeoutHandler());
	}
	
	static ConcurrentHashMap<String, HttpServiceInfo> serviceInfoMap = new ConcurrentHashMap<String, HttpServiceInfo>();

	public static HttpServiceInfo getServiceInfo(String serviceUrl) {
		if (!serviceInfoMap.containsKey(serviceUrl)) {
			HttpServiceInfo info = new HttpServiceInfo(serviceUrl);
			serviceInfoMap.putIfAbsent(serviceUrl, info);
		}
		return serviceInfoMap.get(serviceUrl);
	}

	private final static class HttpRequestInfo {
		boolean valid = true;
		String url;
		HttpResponseCallback fcallback;
			
		HttpRequestInfo() {
		}

		boolean isValid() {
			return valid;
		}

		void setNotValid() {
			valid = false;
		}

	}

	private static TimeoutDetector<HttpRequestInfo> td = null;

	private static class RequestInfoTimeoutHandler implements TimeoutHandler<HttpRequestInfo> {

		public void handle(HttpRequestInfo ri, long expireTime) {
			if (ri.isValid()) {
				ri.setNotValid();
				ri.fcallback.onTimeout(ri.url);
			}
		}
	}


	public static HttpClient getHttpClient() {
		if (null != client) {
			if (!client.isStarted()) {
				try {
					client.start();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			return client;
		}
		synchronized (lock) {
			if (null == client) {
				client = new HttpClient();
				client.setSoTimeout(500);
				client.setTimeout(5000);
				client.setIdleTimeout(1000);
				client.setMaxRetries(1);
				client.setMaxConnectionsPerAddress(512);
				client.setThreadPool(pool);
				client.setConnectorType(HttpClient.CONNECTOR_SELECT_CHANNEL);
			}
		}
		if (!client.isStarted()) {
			try {
				client.start();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return client;
	}

	public void asyncSendGetRuest(final String serviceUrl, int timeoutMill,final HttpResponseCallback callback) throws IOException {
		final HttpResponseCallback fcallback=callback;
		ContentExchange exchange = new ContentExchange() {
			protected void onResponseComplete() throws IOException {
				super.onResponseComplete();
				final HttpRequestInfo ri = (HttpRequestInfo) this.getAttribute(ATTR_REQUEST_INFO);
				
				ri.url = serviceUrl;
				ri.setNotValid();
				int code = getResponseStatus();
				if ( code == 200) {
					try {
						fcallback.onResponse(getResponseContent());
					} catch (Throwable e) {
						fcallback.onRespError(e.toString());
					}					
				} else {
					fcallback.onRespError(code);
				}
			}

			/**
			 * Called when the request headers has been sent
			 * 
			 * @throws IOException
			 */
			protected void onRequestCommitted() throws IOException {
				
			}

			/**
			 * Called when the request and it's body have been sent.
			 * 
			 * @throws IOException
			 */
			protected void onRequestComplete() throws IOException {
				
			}

			protected void onConnectionFailed(Throwable ex) {
				final HttpRequestInfo ri = (HttpRequestInfo) this.getAttribute(ATTR_REQUEST_INFO);
				if (ri.isValid()) {
					ri.setNotValid();
					fcallback.onSendError(ri.url,"onConnectionFailed "+ex);
				}
			}

			protected void onException(Throwable ex) {
				final HttpRequestInfo ri = (HttpRequestInfo) this.getAttribute(ATTR_REQUEST_INFO);
				if (ri.isValid()) {
					ri.setNotValid();
					fcallback.onSendError(ri.url,ex.toString());
				}
			}

			protected void onExpire() {
				final HttpRequestInfo ri = (HttpRequestInfo) this.getAttribute(ATTR_REQUEST_INFO);
				if (ri.isValid()) {
					ri.setNotValid();
					fcallback.onSendError(ri.url,"onExpire");
				}
			}
		};
		exchange.setMethod("GET");
		exchange.setURL(serviceUrl);
		exchange.setVersion(HttpVersions.HTTP_1_1);
		exchange.addRequestHeader("HOST", getServiceInfo(serviceUrl).host + ":" + getServiceInfo(serviceUrl).port);
		System.out.println(getServiceInfo(serviceUrl).host + ":" + getServiceInfo(serviceUrl).port);
		//exchange.addRequestHeader("User-Agent", "asyncClient");
		final HttpRequestInfo ri = new HttpRequestInfo();
		ri.fcallback = callback;
		td.addWithTimeout(ri, timeoutMill);
		exchange.setAttribute(ATTR_REQUEST_INFO, ri);
		getHttpClient().send(exchange);
	}


}
