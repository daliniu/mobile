package com.duowan.taf.proxy;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

import com.duowan.taf.proxy.utils.NameThreadFactory;

/**
 * 时间戳生成器
 * @author albertzhu
 *
 */
public class TimerProvider {

	static ScheduledExecutorService reportTaskExecutor;

	static AtomicLong time = new AtomicLong(System.currentTimeMillis());

	static {
		reportTaskExecutor = Executors.newScheduledThreadPool(1,new NameThreadFactory("TimeProvider"));
		reportTaskExecutor.scheduleWithFixedDelay(new Runnable() {
			public void run() {
				time.set(System.currentTimeMillis());
			}
		}, 10, 10, TimeUnit.MILLISECONDS);
	}
	
	/**
	 * 获取当前时间 间隔是10毫秒
	 * @return
	 */
	public static long getNow() {
		return time.get();
	}

}
