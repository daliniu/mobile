package com.qq.taf.proxy;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import com.qq.taf.proxy.utils.NameThreadFactory;

public class TafTimer {
	
	private final static int reportNodeInterv = 10000 * 1;
	
	private static volatile ScheduledExecutorService reportTaskExecutor;
	
	private static AtomicBoolean isInitProxyStat = new AtomicBoolean(false);
	
	private static AtomicBoolean isInitServerStat = new AtomicBoolean(false);
	
	private static AtomicBoolean isInitServiceRefresh = new AtomicBoolean(false);
	
	/**
	 * 定期上报模块间调用信息
	 */
	private static void initReportStatThread(String threadName, Runnable statThead, long inteval) {
		if (null == reportTaskExecutor) {
			synchronized(TafTimer.class) {
				if(null == reportTaskExecutor) {
					reportTaskExecutor = Executors.newScheduledThreadPool(2, new NameThreadFactory("StatReporter"));
				}
			}
		}
		reportTaskExecutor.scheduleWithFixedDelay(statThead, inteval, inteval,
				TimeUnit.MILLISECONDS);
		TafLoggerCenter.info(threadName+" inited for " + inteval);
	}
	
	public static void initProxyStat(Runnable thread) {
		if(isInitProxyStat.compareAndSet(false, true)) {
			initReportStatThread("ProxyStatReport", thread, reportNodeInterv);
		}
	}
	
	public static void initServerStat(Runnable thread) {
		if(isInitServerStat.compareAndSet(false, true)) {
			initReportStatThread("ServerStatReport", thread, reportNodeInterv);
		}
	}
	
	public static void initServiceRefresher() {
		if(isInitServiceRefresh.compareAndSet(false, true)) {
			initReportStatThread("ServiceRefresher", ServiceInfosRefresher.INSTANCE, ServiceInfosRefresher.getRefreshInterval());
		}
	}
	
	
}
