package com.duowan.taf.proxy.exec;

import com.duowan.taf.cnst.JCESERVERDECODEERR;
import com.duowan.taf.cnst.JCESERVERENCODEERR;
import com.duowan.taf.cnst.JCESERVERNOFUNCERR;
import com.duowan.taf.cnst.JCESERVERNOSERVANTERR;
import com.duowan.taf.cnst.JCESERVERQUEUETIMEOUT;
import com.duowan.taf.cnst.JCESERVERRESETGRID;


public class TafException extends RuntimeException {
	/*
	public final static int SERVER_SUCCESS = 0; // 服务器端处理成功
	public final static int SERVER_DECODEERR = -1; // 服务器端解码异常
	public final static int SERVER_ENCODEERR = -2; // 服务器端编码异常
	public final static int SERVER_NOFUNCERR = -3; // 服务器端没有该函数
	public final static int SERVER_NOSERVANTERR = -4; // 服务器端没有该Servant对象
	public final static int SERVER_OVERLOAD = -5; // 服务器端过载保护
	public final static int JCESERVERQUEUETIMEOUT = -6; // 服务器队列超过限制
	public final static int JCEASYNCCALLTIMEOUT = -7; // 异步调用超时
	public final static int SERVER_UNKNOWNERR = -99; // 服务器端位置异常
	
	public final static int CONN_ERR = -80; // 服务器端位置异常
	*/
	int iRet;

	public TafException() {
		super();
	}

	public TafException(int iRet) {
		super("server error code :" + iRet);
		this.iRet = iRet;
	}

	public TafException(String message) {
		super(message);
	}

	public TafException(String message, Throwable cause) {
		super(message, cause);
	}

	public TafException(Throwable cause) {
		super(cause);
	}
	
	public static TafException makeException(int iRet,String msg) {
		if (iRet == JCESERVERDECODEERR.value) {
			return new TafServerDecodeException(iRet);
		} else if (iRet == JCESERVERENCODEERR.value) {
			return new TafServerEncodeException(iRet);
		} else if (iRet == JCESERVERNOFUNCERR.value) {
			return new TafServerNoFuncException(iRet);
		} else if (iRet == JCESERVERNOSERVANTERR.value) {
			return new TafServerNoServantException(iRet);
		} else if (iRet == JCESERVERQUEUETIMEOUT.value) {
			return new TafServerQueueTimeoutException(iRet);
		} else if (iRet == JCESERVERRESETGRID.value) {
			return new TafServerResetGridException(iRet);
		} else {
			return new TafServerUnknownException(iRet);
		}
	}

	public static TafException makeException(int iRet) {
		return makeException(iRet,"");
	}

	public int getIRet() {
		return iRet;
	}

}
