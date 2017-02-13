package com.duowan.taf;

public interface PingFPrx extends  com.duowan.taf.proxy.ServantInterface
{
    public void tafPing();
    
    public void tafPing(java.util.Map __ctx);
    
    public void async_tafPing(com.duowan.taf.QueryFPrxCallback callback);
    
    public void async_tafPing(com.duowan.taf.QueryFPrxCallback callback,java.util.Map __ctx);
      

}
