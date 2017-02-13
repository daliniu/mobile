package com.qq.taf.proxy.test.router.Test;

public interface HelloPrx
{
    public int testHello(String s, com.qq.taf.holder.JceStringHolder r);

    public int testHello(String s, com.qq.taf.holder.JceStringHolder r, java.util.Map __ctx);

    public void async_testHello(com.qq.taf.proxy.test.router.Test.HelloPrxCallback callback, String s);

    public void async_testHello(com.qq.taf.proxy.test.router.Test.HelloPrxCallback callback, String s, java.util.Map __ctx);


}
