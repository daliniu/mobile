package com.qq.taf.proxy;

import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * 模块间调用信息的一个线程安全包装
 * @author albertzhu
 *
 */
public class SendMsgStatBody {
	
	public final static int CallStatusSucc = 0 ;
	public final static int CallStatusExec = 1 ;
	public final static int CallStatusTimeout = 2 ;
	
	AtomicInteger count = new AtomicInteger();
	AtomicInteger execCount = new AtomicInteger();
	AtomicInteger timeoutCount = new AtomicInteger();
	public ConcurrentHashMap<Integer, AtomicInteger> intervalCount = new ConcurrentHashMap<Integer, AtomicInteger>();
	ReentrantReadWriteLock intervListlock = new ReentrantReadWriteLock();
	CopyOnWriteArrayList<Integer> timeStatInterv = new CopyOnWriteArrayList<Integer>();

	public SendMsgStatBody(List<Integer> _timeStatInterv) {
		setPointStatInterv(_timeStatInterv);
	}

	public void setPointStatInterv(List<Integer> _timeStatInterv) {
		timeStatInterv.clear();
		timeStatInterv.addAll(_timeStatInterv);
		for (Integer i : timeStatInterv) {
			if (!intervalCount.containsKey(i)) {
				intervalCount.put(i, new AtomicInteger());
			}
		}
	}

	public void onCallFinished(long costTimeMin, int callStatus) {
		if (callStatus == CallStatusSucc) {
			count.incrementAndGet();
			totalRspTime.addAndGet(costTimeMin);
			if (maxRspTime.get() < costTimeMin) {
				maxRspTime.set((int) costTimeMin);
			} else if (costTimeMin > 0 && costTimeMin < minRspTime.get()) {
				minRspTime.set((int) costTimeMin);
			}
		} else if (callStatus == CallStatusExec) {
			execCount.incrementAndGet();
		} else if (callStatus == CallStatusTimeout) {
			timeoutCount.incrementAndGet();
		}
		for (int i = 0; i < timeStatInterv.size(); i++) {
			if (costTimeMin <= timeStatInterv.get(i)) {
				intervalCount.get(timeStatInterv.get(i)).incrementAndGet();
				break;
			}
		}
	}

	AtomicLong totalRspTime = new AtomicLong(); // 调用总时间用来计算平均时间
	AtomicInteger maxRspTime = new AtomicInteger(); // 最大响应时间
	AtomicInteger minRspTime = new AtomicInteger(); // 最小响应时间


	public java.lang.Object clone() {
		java.lang.Object o = null;
		try {
			o = super.clone();
		} catch (CloneNotSupportedException ex) {
			assert false; // impossible
		}
		return o;
	}

	public void clearStatData() {
		count.set(0);
		execCount.set(0);
		timeoutCount.set(0);
		for (Entry<Integer, AtomicInteger> entry : intervalCount.entrySet()) {
			entry.getValue().set(0);
		}
		totalRspTime.set(0);
		maxRspTime.set(0);
		minRspTime.set(0);
	}

	

	public int getCount() {
		return count.get();
	}

	public int getExecCount() {
		return execCount.get();
	}

	public int getTimeoutCount() {
		return timeoutCount.get();
	}

	public long getTotalRspTime() {
		return totalRspTime.get();
	}

	public int getMaxRspTime() {
		return maxRspTime.get();
	}

	public int getMinRspTime() {
		return minRspTime.get();
	}

}
