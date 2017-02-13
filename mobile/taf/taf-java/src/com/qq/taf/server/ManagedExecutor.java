package com.qq.taf.server;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.LinkedBlockingQueue;

import com.qq.sim.Millis100TimeProvider;

/**
 * 受控的执行器
 * @author fanzhang
 */
public class ManagedExecutor implements Executor,Runnable {
	
	private String name;
	private int queueTimeout;
	private BlockingQueue<TaskWrapper> blockingQueue;
	
	private class TaskWrapper {
		private Runnable task;
		private long timeoutPoint;
		public TaskWrapper(Runnable task) {
			this.task=task;
			timeoutPoint=Millis100TimeProvider.INSTANCE.currentTimeMillis()+queueTimeout;
		}
		public void doTask() {
			if(Millis100TimeProvider.INSTANCE.currentTimeMillis()<timeoutPoint) task.run();
			else SystemLogger.record("ManagedExecutor|task is timeout in queue|"+name);
		}
	}
	
	public ManagedExecutor(String name,int queueCapacity,int queueTimeout,int threads) {
		this.name=name;
		this.queueTimeout=queueTimeout;
		blockingQueue=new LinkedBlockingQueue<TaskWrapper>(queueCapacity);
		for(int i=0;i<threads;i++) new Thread(this,name+"_"+i).start();
	}

	public void execute(Runnable task) {
		boolean success=blockingQueue.offer(new TaskWrapper(task));
		if(!success) SystemLogger.record("ManagedExecutor|queue is full|"+name);
	}
	
	public int getQueueSize() {
		return blockingQueue.size();
	}
	
	public void run() {
		while(true) {
			try {
				blockingQueue.take().doTask();
			} catch(Throwable t) {
				SystemLogger.record("ErrorFound", t);
			}
		}
	}

}
