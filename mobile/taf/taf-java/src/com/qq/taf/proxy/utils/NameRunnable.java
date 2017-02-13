
package com.qq.taf.proxy.utils;

/**
 * 可命名的Runnable
 * 便于在jmx中查看
 * @author albertzhu
 *
 */
public class NameRunnable implements Runnable {

	private final String newName;

	private final Runnable runnable;

	public NameRunnable(Runnable runnable, String newName) {
		this.runnable = runnable;
		this.newName = newName;
	}

	public void run() {
		Thread currentThread = Thread.currentThread();
		String oldName = currentThread.getName();

		if (newName != null) {
			setName(currentThread, newName);
		}

		try {
			runnable.run();
		} finally {
			setName(currentThread, oldName);
		}
	}

	private void setName(Thread thread, String name) {
		try {
			thread.setName(name);
		} catch (SecurityException se) {
			se.printStackTrace();
		}
	}
}
