package com.duowan.taf;

import java.util.Map;

public class PingFPrxHelper extends com.duowan.taf.proxy.ServantProxy implements PingFPrx {

	public PingFPrxHelper taf_hash(int hashCode) {
		super.taf_hash(hashCode);
		return this;
	}

	public java.util.Map __defaultContext() {
		java.util.HashMap _ctx = new java.util.HashMap();
		return _ctx;
	}

	protected String sServerEncoding = "GBK";

	public int setServerEncoding(String se) {
		sServerEncoding = se;
		return 0;
	}

	public void tafPing() {
		 tafPing(__defaultContext());
	}

	public void tafPing(Map __ctx) {
		com.duowan.taf.jce.JceOutputStream _os = new com.duowan.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        byte[] _sBuffer = com.duowan.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());
        byte[] _returnSBuffer = taf_invoke("taf_ping", _sBuffer, __ctx);
	}
	
	public void async_tafPing(QueryFPrxCallback callback) {
		async_tafPing(callback, __defaultContext());
	}

	public void async_tafPing(QueryFPrxCallback callback, Map __ctx) {
        com.duowan.taf.jce.JceOutputStream _os = new com.duowan.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        byte[] _sBuffer = com.duowan.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());
        taf_invokeAsync(callback, "taf_ping", _sBuffer, __ctx);
	}

	

}
