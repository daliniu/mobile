package com.qq.taf.proxy.test;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

import com.qq.jutil.j4log.Logger;
import com.qq.taf.proxy.utils.NameRunnable;

public abstract class BechmarkFrameWork {

	final Logger logger = Logger.getLogger("BechmarkLog");

	public boolean isRunning = true ;
	
	AtomicLong completeCount = new AtomicLong();
	
	AtomicLong writeCount = new AtomicLong();

	AtomicLong errorCount = new AtomicLong();

	private volatile ScheduledExecutorService taskExecutor = Executors.newScheduledThreadPool(1);

	ExecutorService es;
	
	public abstract Runnable getTestTask(); 
			
	public abstract int getTestThreadCount();
	
	public abstract int getMonitorInterv();
	
	public abstract int getTestMinute();
	
	public void benchmarkFinished() {
		
	}
	
	public boolean isRunning() {
		return isRunning;
	}
	
	public void setRunning(boolean isRunning) {
		this.isRunning = isRunning;
	}

	public void startTest() {
		try {
			start();
			long startTime = System.currentTimeMillis();
			while ( true ) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				if ( System.currentTimeMillis() -  startTime > (getTestMinute()*1000*60)) {
					setRunning(false);
					logger.info(getBechmarkResult());
					logger.info("stopping test");
					Thread.sleep(10000);
					shutDown();
					logger.info("stopped test");
					break;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		benchmarkFinished();
	}
	
	public void start() {
		es = Executors.newFixedThreadPool(getTestThreadCount());
		for (int i = 0; i < getTestThreadCount(); i++) {
			es.submit(new NameRunnable(getTestTask(),"BenchmarkThread"+i));
		}
		startMonitorer();
	}

	public void startMonitorer() {
		taskExecutor.scheduleWithFixedDelay(new NameRunnable(new MonitorTask(),"BenchmarkMonitorTask"), getMonitorInterv() , getMonitorInterv() ,
				TimeUnit.MILLISECONDS);
	}

	public void shutDown() {
		if ( null != es ) {
			try {
				es.shutdownNow();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		if ( null != taskExecutor ) {
			try {
				taskExecutor.shutdownNow();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	public long writeCountIncrementAndGet() {
		return writeCount.incrementAndGet();
	}
	
	public long completeIncrementAndGet() {
		return completeCount.incrementAndGet();
	}
	
	public long ErrorIncrementAndGet() {
		return errorCount.incrementAndGet();
	}
	
	public long getWriteCount() {
		return writeCount.get();
	}
	
	public long getCompleteCount() {
		return completeCount.get() ;
	}
	
	public long getErrCount() {
		return errorCount.get() ;
	}
	
	public String getMonitorPrint() {
		return "";
	}
		
	public String getBechmarkResult() {
		StringBuffer sb = new StringBuffer();
		sb.append("completecount:"+completeCount.get()+"|errorCount:"+errorCount.get());
		return sb.toString();
	}
	
	class MonitorTask implements Runnable {
		long lastCompleteCount = 0;
		long lastWriteCount = 0 ;

		public void run() {
			long now = completeCount.get();
			long writed = writeCount.get();
			logger.info("thought:" + (now - lastCompleteCount) + " writeThought:"+(writed-lastWriteCount) + " errorCount:"+errorCount.get()+" "+getMonitorPrint());
			lastCompleteCount = now;
			lastWriteCount = writed;
		}

	}



}
