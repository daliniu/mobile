package com.qq.taf.proxy.test.router.Test;

public final class HelloPrxHelper extends com.qq.taf.proxy.ServantProxy implements HelloPrx
{

    public HelloPrxHelper taf_hash(int hashCode)
    {
        super.taf_hash(hashCode);
        return this;
    }

    public java.util.Map __defaultContext()
    {
        java.util.HashMap _ctx = new java.util.HashMap();
        return _ctx;
    }

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public int testHello(String s, com.qq.taf.holder.JceStringHolder r)
    {
        return testHello(s, r, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testHello(String s, com.qq.taf.holder.JceStringHolder r, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(s, 1);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();
        status.put(com.qq.taf.cnst.Const.STATUS_GRID_KEY, s);

        byte[] _returnSBuffer = taf_invoke("testHello", _sBuffer, __ctx, status);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        r.value = "";
        r.value = (String) _is.read(r.value, 2, true);
        return _ret;
    }

    public void async_testHello(com.qq.taf.proxy.test.router.Test.HelloPrxCallback callback, String s)
    {
        async_testHello(callback, s, __defaultContext());
    }

    public void async_testHello(com.qq.taf.proxy.test.router.Test.HelloPrxCallback callback, String s, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(s, 1);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        java.util.HashMap<String, String> status = new java.util.HashMap<String, String>();
        status.put(com.qq.taf.cnst.Const.STATUS_GRID_KEY, s);

        taf_invokeAsync(callback, "testHello", _sBuffer, __ctx, status);

    }

}
