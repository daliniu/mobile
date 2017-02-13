package com.qq.taf.proxy.test.Test;

public final class APrx extends com.qq.taf.proxy.ServantProxy
{

    public APrx taf_hash(int hashCode)
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

    public int test(long p, int i)
    {
        return test(p, i, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int test(long p, int i, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(p, 1);
        _os.write(i, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("test", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        return _ret;
    }

    public void async_test(com.qq.taf.proxy.test.Test.APrxCallback callback, long p, int i)
    {
        async_test(callback, p, i, __defaultContext());
    }

    public void async_test(com.qq.taf.proxy.test.Test.APrxCallback callback, long p, int i, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(p, 1);
        _os.write(i, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "test", _sBuffer, __ctx);

    }

}
