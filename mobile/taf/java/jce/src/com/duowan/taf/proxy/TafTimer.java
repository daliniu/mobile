package com.duowan.taf.proxy;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import com.duowan.taf.proxy.utils.NameThreadFactory;

public class TafTimer {
	
	private final static int reportNodeInterv = 10000 * 1;
	
	private static volatile ScheduledExecutorService reportTaskExecutor;
	
	private static AtomicBoolean isInitProxyStat = new AtomicBoolean(false);
	
	private static AtomicBoolean isInitServerStat = new AtomicBoolean(false);
	/**
	 * 定期上报模块间调用信息
	 */
	private static void initReportStatThread(Runnable statThead) {
		if (null == reportTaskExecutor) {
			synchronized(TafTimer.class) {
				if(null == reportTaskExecutor) {
					reportTaskExecutor = Executors.newScheduledThreadPool(1,new NameThreadFactory("StatReporter"));
				}
			}
		}
		reportTaskExecutor.scheduleWithFixedDelay(statThead, reportNodeInterv, reportNodeInterv,
				TimeUnit.MILLISECONDS);
		TafLoggerCenter.info("ReportStatWorker inited for " + reportNodeInterv);
	}
	
	public static void initProxyStat(Runnable thread) {
		if(isInitProxyStat.compareAndSet(false, true)) {
			initReportStatThread(thread);
		}
	}
	
	public static void initServerStat(Runnable thread) {
		if(isInitServerStat.compareAndSet(false, true)) {
			initReportStatThread(thread);
		}
	}
}
