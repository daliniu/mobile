package com.qq.taf;

public interface PingFPrx extends  com.qq.taf.proxy.ServantInterface
{
    public void tafPing();
    
    public void tafPing(java.util.Map __ctx);
    
    public void async_tafPing(com.qq.taf.QueryFPrxCallback callback);
    
    public void async_tafPing(com.qq.taf.QueryFPrxCallback callback,java.util.Map __ctx);
      

}
