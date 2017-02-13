package com.qq.taf.proxy.utils;

import java.security.SecureRandom;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.qq.sim.Millis100TimeProvider;
import com.qq.taf.proxy.TafLoggerCenter;



/**
 * 自meteorchen的advcache修改而来 过期自动淘汰的cache
 * 
 * @author albertzhu
 * 
 * @param <K>
 * @param <V>
 */
public class TimeoutCache<K, V> {

	private ConcurrentHashMap<K, TimeoutNode<K, V>> map = null; // 保存cache的key-value对应关系的map
	ReentrantReadWriteLock lock = new ReentrantReadWriteLock(); // 锁
	private volatile ScheduledExecutorService taskExecutor;
	final ExecutorService timeoutScheduler = Executors.newFixedThreadPool(2,new NameThreadFactory("TimeoutCacheWorker"));
	TimeoutNodeList<K, V> timeoutNodeList = new TimeoutNodeList<K, V>();									// 最后访问时间排序的链表
	/**
	 * cache初始化大小
	 * @param initSize
	 */
	public TimeoutCache(int initSize) {
		map = new ConcurrentHashMap<K, TimeoutNode<K, V>>(initSize);
		taskExecutor = Executors.newScheduledThreadPool(1,new NameThreadFactory("TimeoutCacheChecker"));
		taskExecutor.scheduleWithFixedDelay(new Runnable() {
			public void run() {
				long start = Millis100TimeProvider.INSTANCE.currentTimeMillis();
				int cleanSize = cleanUp();
				if ( cleanSize > 0  ) {
					TafLoggerCenter.info("cleanSize:"+cleanSize+" cost:"+(Millis100TimeProvider.INSTANCE.currentTimeMillis()-start));
				}
			}
		}, 500, 500, TimeUnit.MILLISECONDS);
	}

	public V get(K key) {
		lock.readLock().lock();
		try {
			TimeoutNode<K, V> n = map.get(key);
			if (n != null) {
				return n.value;
			}
		} finally {
			lock.readLock().unlock();
		}
		return null;
	}

	public boolean containsCache(K key) {
		return map.containsKey(key);
	}

	public V put(K key, V value, TimeoutHandler<K,V> handler ,long aliveTime) {
		TimeoutNode<K, V> n = new TimeoutNode<K, V>(key, value, handler ,Millis100TimeProvider.INSTANCE.currentTimeMillis(), aliveTime);
		n = put(n);
		return n != null ? n.value : null;
	}

	private TimeoutNode<K, V> put(TimeoutNode<K, V> n) {
		lock.writeLock().lock();
		try {
			TimeoutNode<K, V> old = map.put(n.key, n);
			timeoutNodeList.add(n);
			if(old != null)
				timeoutNodeList.remove(old);
			return old;
		} finally {
			lock.writeLock().unlock();
		}
	}

	public void clear() {
		lock.writeLock().lock();
		try {
			map.clear();
			timeoutNodeList.clear();
		} finally {
			lock.writeLock().unlock();
		}
	}

	public V remove(K key) {
		lock.writeLock().lock();
		try {
			TimeoutNode<K, V> n = map.remove(key);
			if(n != null){
				timeoutNodeList.remove(n);
				return n.value;
			}
			return null;
		} finally {
			lock.writeLock().unlock();
		}
	}

	V remove(TimeoutNode<K, V> n) {
		lock.writeLock().lock();
		try {
			TimeoutNode<K, V> n1 = map.get(n.key);
			if(n1 == n){
				map.remove(n.key);
				timeoutNodeList.remove(n);
				return n.value;
			}
			return null;
		} finally {
			lock.writeLock().unlock();
		}
	}

	public int size() {
		return map.size();
	}

	public Iterator<Entry<K, V>> iterator() {
		return new TimeoutCacheIterator<K, V>(this);
	}
	/**
	 * 清除元素
	 * 
	 * @param h
	 *            清除前需要做的操作
	 * @return 清除的元素数目
	 */
	public int cleanUp() {
		int c1 = 0;
		try {
			Set<Map.Entry<K, TimeoutNode<K, V>>> entrySet = map.entrySet();
			Iterator<Map.Entry<K, TimeoutNode<K, V>>> it = entrySet.iterator();
			while (it.hasNext()) {
				Map.Entry<K, TimeoutNode<K, V>> n = it.next();
				final TimeoutNode<K, V> node = n.getValue();
				final long now =Millis100TimeProvider.INSTANCE.currentTimeMillis() - node.createTime;
				if(now >= node.aliveTime){
					remove(node);
					timeoutScheduler.submit(new Runnable() {
						public void run() {
							node.handler.timeout(node.getKey(), node.getValue(), now);
						}	
					});
					++c1;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.err.print("cleanTimeoutItem error" + e);
		}
		return c1;
	}

	public Set<K> keySet() {
		return map.keySet();
	}

	public static void main(String[] args) {
		int putThreadCount = 5 ;
		ExecutorService es = Executors.newFixedThreadPool(putThreadCount);
		final SecureRandom random = new SecureRandom();
		final TimeoutCache<String,String> cache = new TimeoutCache<String,String>(4096);
		for (int i = 0; i < putThreadCount; i++) {
			es.submit(new NameRunnable(new Runnable() {
				public void run() {
					long start = Millis100TimeProvider.INSTANCE.currentTimeMillis();
					NullTimeoutHandler<String,String> nullHandler = new NullTimeoutHandler<String,String>();
					for ( int i = 0 ; i < 100000 ; i++ ) {
						long r = random.nextInt(10000);
						cache.put("key:"+r, "key", nullHandler,r);
					}
					System.out.println("add cost "+(Millis100TimeProvider.INSTANCE.currentTimeMillis()-start));
				}
			},"BenchmarkThread"+i));
		}
		try {
			Thread.sleep(60000*10);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
}
