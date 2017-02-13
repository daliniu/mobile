package com.qq.taf.server;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.ServantProxyThreadData;
import com.qq.taf.proxy.TimerProvider;
import com.qq.taf.server.jce2java.LogPrx;
import com.qq.taf.server.jce2java.LogPrxHelper;
import com.qq.taf.server.util.FileUtil;
import com.qq.taf.server.util.GeneralException;

/**
 * 时间日志记录器
 * @author fanzhang
 */
public class LoggerFactory {
	
	public static class Logger {
		
		private Recorder recorder;
		
		public Logger(Recorder recorder) {
			this.recorder=recorder;
		}
		
		public void log(String content) {
			recorder.record(LevelX.MAX.value, content);
		}
		
		public void info(String content) {
			recorder.record(Level.INFO.value,content);
		}
		
		public void debug(String content) {
			recorder.record(Level.DEBUG.value,content);
		}
		
		public void error(String content) {
			recorder.record(Level.ERROR.value,content);
		}
		
		public void warn(String content) {
			recorder.record(Level.WARN.value,content);
		}
		
		public void log(String content,Throwable th) {
			log(content+"\n"+getStackTrace(th));
		}
		
		public void info(String content,Throwable th) {
			info(content+"\n"+getStackTrace(th));
		}
		
		public void debug(String content,Throwable th) {
			debug(content+"\n"+getStackTrace(th));
		}
		
		public void error(String content,Throwable th) {
			error(content+"\n"+getStackTrace(th));
		}
		
		public void warn(String content,Throwable th) {
			warn(content+"\n"+getStackTrace(th));
		}
		
		private String getStackTrace(Throwable th) {
			StringWriter sw=new StringWriter();
			th.printStackTrace(new PrintWriter(sw));
			return sw.toString();
		}
		
	}
	
	public static final String FORMAT_DAY="%Y%m%d";
	public static final String FORMAT_HOUR="%Y%m%d.%H";
	public static final String FORMAT_MINUTE="%Y%m%d.%H%M";
	
	public static enum Level {
		
		DEBUG(0),INFO(1),WARN(2),ERROR(3);
		
		public int value;
		
		Level(int value) {
			this.value=value;
		}
		
	}
	
	private static enum LevelX {
		
		MIN(-1),MAX(4);
		
		public int value;
		
		LevelX(int value) {
			this.value=value;
		}
			
	}
	
	private static final String[] LEVELS={"|DEBUG","|INFO","|WARN","|ERROR","|-"};
	
	public static enum Type {
		
		LOCAL(-1),REMOTE(1),BOTH(0);
		
		public int value;
		
		Type(int value) {
			this.value=value;
		}
		
	}
	
	private static final LoggerFactory INSTANCE=new LoggerFactory();
	
	private static class Content {
		
		public long time;
		public int level;
		public String content;
		
		public Content(long time,int level,String content) {
			this.time=time;
			this.level=level;
			this.content=content;
		}
		
	}
	
	private static class Recorder {
		
		private static final SimpleDateFormat DATE_FORMAT=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.S"); 
		public String logName;
		public String format;
		private String format2;
		private int level;
		private int type;
		public ConcurrentLinkedQueue<Content> queue;
		private DyeingRecorder dyeingRecorder;
		
		public Recorder(String logName,String format,int level,int type,DyeingRecorder dyeingRecorder) {
			this.logName=logName;
			this.format=format;
			this.format2=format.replaceAll("%", "%1\\$t");
			this.level=level;
			this.type=type;
			queue=new ConcurrentLinkedQueue<Content>();
			this.dyeingRecorder=dyeingRecorder;
		}
		
		public void record(int level,String content) {
			if(level>=this.level) {
				Content objContent=new Content(TimerProvider.getNow(),level,content);
				queue.offer(objContent);
				dyeingRecorder.record(objContent);
			}
		}
		
		public List<String> getLines(int batchCount) {
			List<String> lines=new ArrayList<String>();
			int count=0;
			Content content=null;
			while(count++<batchCount && (content=queue.poll())!=null) lines.add(makeLine(content));
			return lines;
		}
		
		public String getFileName(long currentTime) {
			return logName+"_"+String.format(format2, currentTime)+".log";
		}
		
		public boolean needLocalWrite() {
			return type<=Type.BOTH.value;
		}
		
		public boolean needRemoteWrite() {
			return type>=Type.BOTH.value;
		}
		
		// this.level is not volatile
		public void setLevel(int level) {
			if(this.level!=LevelX.MIN.value) this.level=level;
		}
		
		protected String makeLine(Content content) {
			return DATE_FORMAT.format(content.time)+(level!=LevelX.MIN.value?LEVELS[content.level]:"")+"|"+content.content+"\n";
		}
		
	}
	
	private static class DyeingRecorder extends Recorder {
		
		public static final String DYEING_LOGGER_NAME="dyeing";
		public static final String DYEING_LOG_PATH="/taf_dyeing";
		public static final String DYEING_APP_NAME="taf_dyeing";
		public static final String DYEING_SERVER_NAME="dyeing";
		private String prefix;
		
		public DyeingRecorder(String appName,String serverName,Map<String,Recorder> recorders) {
			super(DYEING_LOGGER_NAME,FORMAT_DAY,LevelX.MIN.value,Type.BOTH.value,null);
			prefix=appName+"."+serverName+"|";
			recorders.put(DYEING_LOGGER_NAME, this);
		}
		
		@Override
		public void record(int level, String content) {
		}
		
		public void record(Content content) {
			//暂时不使用
//			String dyeingKey=ServantProxyThreadData.INSTANCE.getData()._dyeingKey;
//			if(dyeingKey!=null && dyeingKey.trim().length()!=0) queue.offer(content);
		}
		
		@Override
		protected String makeLine(Content content) {
			return prefix+super.makeLine(content);
		}
		
	}
	
	private static class LocalWriter extends Thread {
		
		private static final int BATCH_COUNT=3000;
		private static final int SLEEP_MILLISECONDS=1000;
		private Map<String,Recorder> recorders;
		private String baseFileName;
		private String baseDyeingFileName;
		private RemoteWriter remoteWriter;
		
		public LocalWriter(Map<String,Recorder> recorders,String logPath,String appName,String serverName,RemoteWriter remoteWriter) {
			String basePath=logPath!=null?(logPath+(appName!=null && serverName!=null?"/"+appName+"/"+serverName+"/":"/")):null;
			if(basePath!=null && basePath.trim().length()!=0) {
				File basePathFile=new File(basePath);
				if(!basePathFile.exists() && !basePathFile.mkdirs()) GeneralException.raise("LoggerFactory|LocalWriter|basePath does not exist and can not be created");
				baseFileName=basePath+(appName!=null && serverName!=null?appName+"."+serverName+"_":"");
			}
			baseDyeingFileName=logPath!=null?logPath+DyeingRecorder.DYEING_LOG_PATH+"/":null;
			this.remoteWriter=remoteWriter;
			this.recorders=recorders;		
			setName("LoggerLocalWriter");
		}
		
		@Override
		public void run() {
			while(true) {
				try {
					long now=TimerProvider.getNow();
					for(String logName:recorders.keySet()) {
						Recorder recorder=recorders.get(logName);
						List<String> lines=recorder.getLines(BATCH_COUNT);
						if(lines.size()==0) continue;
						if(recorder.needLocalWrite()) {
							String recorderFileName=recorder.getFileName(now);
							String fileName=(recorderFileName.startsWith(DyeingRecorder.DYEING_LOGGER_NAME)?baseDyeingFileName:baseFileName)+recorderFileName;
							FileUtil.appendLinesToFile(fileName, lines);
						}
						if(recorder.needRemoteWrite()) remoteWriter.queue.offer(new RemoteData(recorder.logName,recorder.format,lines)); 
					}
					sleep(SLEEP_MILLISECONDS);
				} catch(Throwable t) {
					System.err.println("LoggerFactory|LocalWriter|error");
					t.printStackTrace();
				}
			}
		}
		
	}
	
	private static class RemoteData {
		public String logName;
		public String format;
		public List<String> lines;
		public RemoteData(String logName,String format,List<String> lines) {
			this.logName=logName;
			this.format=format;
			this.lines=lines;
		}
	}
	
	private static class RemoteWriter extends Thread {
		
		private static final int BATCH_LOOPS=100;
		private static final String[] SS={};
		private static final int SLEEP_MILLISECONDS=1000;
		private LogPrx logPrx;
		private String appName;
		private String serverName;
		public ConcurrentLinkedQueue<RemoteData> queue;
		
		public RemoteWriter(Communicator comm,String obj,String appName,String serverName) {
			if(comm!=null) {
				this.logPrx=comm.stringToProxy(obj, LogPrxHelper.class);
				((LogPrxHelper)this.logPrx).taf_sync_timeout(3000);
			}
			this.appName=appName;
			this.serverName=serverName;
			queue=new ConcurrentLinkedQueue<RemoteData>();
			setName("LoggerRemoteWriter");
		}
		
		@Override
		public void run() {
			while(true) {
				try {
					for(int loops=0;loops<BATCH_LOOPS;loops++) {
						RemoteData remoteData=queue.poll();
						if(remoteData==null) break;
						if(remoteData.logName.startsWith(DyeingRecorder.DYEING_LOGGER_NAME)) 
							logPrx.logger(DyeingRecorder.DYEING_APP_NAME, DyeingRecorder.DYEING_SERVER_NAME, "", remoteData.format, remoteData.lines.toArray(SS));
						else 
							logPrx.logger(appName, serverName, remoteData.logName, remoteData.format, remoteData.lines.toArray(SS));
					}
					sleep(SLEEP_MILLISECONDS);
				} catch(Throwable t) {
					System.err.println("LoggerFactory|RemoteWriter|error");
					t.printStackTrace();
				}
			}
		}
		
	}
	
	private static class QueueMonitor extends Thread {
		
		private static final int SLEEP_MILLISECONDS=300000;
		public static volatile int maxLogQueueSize=450000;
		public static volatile int maxRemoteWriteQueueSize=15000;
		private Map<String,Recorder> recorders;
		private RemoteWriter remoteWriter;
		
		public QueueMonitor(Map<String,Recorder> recorders,RemoteWriter remoteWriter) {
			this.recorders=recorders;
			this.remoteWriter=remoteWriter;
			setName("LoggerQueueMonitor");
		}
		
		@Override
		public void run() {
			while(true) {
				try {
					for(String logName:recorders.keySet()) {
						Recorder recorder=recorders.get(logName);
						if(recorder.queue.size()>maxLogQueueSize) {
							recorder.queue.clear();
							System.err.println("LoggerFactory|QueueMonitor|error|queue for ["+logName+"] is overhead");
						}
					}
					if(remoteWriter.queue.size()>maxRemoteWriteQueueSize) {
						remoteWriter.queue.clear();
						System.err.println("LoggerFactory|QueueMonitor|error|queue for [<RemoteWriter>] is overhead");
					}
					sleep(SLEEP_MILLISECONDS);
				} catch(Throwable t) {
					System.err.println("LoggerFactory|QueueMonitor|error");
					t.printStackTrace();
				}
			}
		}
		
	}
	
	private Map<String, Recorder> recorders;
	private Map<String, Logger> loggers;
	private RemoteWriter remoteWriter;
	private LocalWriter localWriter;
	private QueueMonitor queueMonitor;
	private boolean configed;
	private DyeingRecorder dyeingRecorder;
	
	private LoggerFactory() {
		recorders=new ConcurrentHashMap<String, Recorder>();
		loggers=new ConcurrentHashMap<String, Logger>();
		configed=false;
	}
	
	public static LoggerFactory getInstance() {
		return INSTANCE;
	}
	
	public synchronized void setConfigInfo(Communicator comm,String obj,String appName,String serverName,String logPath) {
		if(configed) GeneralException.raise("LoggerFactory|setConfigInfo|error|this method should be called once at most");
		dyeingRecorder=new DyeingRecorder(appName,serverName,recorders);
		remoteWriter=new RemoteWriter(comm,obj,appName,serverName);
		localWriter=new LocalWriter(recorders,logPath,appName,serverName,remoteWriter);
		queueMonitor=new QueueMonitor(recorders,remoteWriter);
		remoteWriter.start();
		localWriter.start();
		queueMonitor.start();
		configed=true;
	}
	
	public synchronized void setConfigInfo(Communicator comm,String obj,String appName,String serverName) {
		setConfigInfo(comm, obj, appName, serverName, null);
	}
	
	public synchronized void setConfigInfo(String logPath) {
		setConfigInfo(null, null, null, null, logPath);
	}
	
	private synchronized void createLogger(String logName,String format,int level,int type) {
		if(!loggers.containsKey(logName)) {
			Recorder recorder=new Recorder(logName,format,level,type,dyeingRecorder);
			recorders.put(logName, recorder);
			Logger logger=new Logger(recorder);
			loggers.put(logName, logger);
		}
	}
	
	public synchronized void createLogger(String logName,String format,Level level,Type type) {
		createLogger(logName, format, level.value, type.value);
	}
	
	public synchronized void createLogger(String logName,String format,Type type) {
		createLogger(logName, format, LevelX.MIN.value, type.value);
	}
	
	public synchronized void createLogger(String logName,Level level,Type type) {
		createLogger(logName, FORMAT_DAY, level.value, type.value);
	}
	
	public synchronized void createLogger(String logName,Type type) {
		createLogger(logName, FORMAT_DAY, LevelX.MIN.value, type.value);
	}
	
	public Logger getLogger(String logName) {
		return loggers.get(logName);
	}
	
	// only for dynamic setting the level at the web console.
	public void setLevel(String level) {
		final Map<String,Integer> map=new HashMap<String, Integer>();
		map.put("INFO", Level.INFO.value);
		map.put("DEBUG", Level.DEBUG.value);
		map.put("WARN", Level.WARN.value);
		map.put("ERROR", Level.ERROR.value);
		map.put("NONE", 100);
		for(Recorder recorder:recorders.values()) recorder.setLevel(map.get(level));
	}
	
	public String queryQueueSize() {
		StringBuilder result=new StringBuilder();
		for(String logName:recorders.keySet()) result.append(String.format("%s : %d\n",logName,recorders.get(logName).queue.size()));
		result.append(String.format("<RemoteWriter> : %d\n",remoteWriter.queue.size()));
		return result.toString();
	}
	
	// only for adjusting queue monitor
	public void setQueueMonitorParams(int maxLogQueueSize,int maxRemoteWriteQueueSize) {
		QueueMonitor.maxLogQueueSize=maxLogQueueSize;
		QueueMonitor.maxRemoteWriteQueueSize=maxRemoteWriteQueueSize;
	}
	
	public static void main(String[] args) throws Throwable {
		System.out.println("LogFactory ...");
		LoggerFactory factory=LoggerFactory.getInstance();
		factory.setConfigInfo("e:/temp");
		factory.createLogger("test", Type.LOCAL);
		Logger logger=factory.getLogger("test");
		for(int i=0;i<100;i++) {
			for(int j=0;j<100;j++) logger.log("test"+j+":"+i);
			Thread.sleep(50);
		}
		System.out.println("LogFactory.");
	}
	
}
