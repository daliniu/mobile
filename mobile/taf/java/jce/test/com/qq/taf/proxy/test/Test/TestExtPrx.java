package com.qq.taf.proxy.test.Test;

public final class TestExtPrx extends com.qq.taf.proxy.ServantProxy
{

    public TestExtPrx taf_hash(int hashCode)
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

    public int testMapStruct(int id, java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapStruct(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapStruct(int id, java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapStruct", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Integer, com.qq.taf.proxy.test.Test.MyInfo>();
        Integer __var_37 = 0;
        com.qq.taf.proxy.test.Test.MyInfo __var_38 = new com.qq.taf.proxy.test.Test.MyInfo();
        mo.value.put(__var_37, __var_38);
        mo.value = (java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapStruct(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mi)
    {
        async_testMapStruct(callback, id, mi, __defaultContext());
    }

    public void async_testMapStruct(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapStruct", _sBuffer, __ctx);

    }

    public int testMyInfo(int id, com.qq.taf.proxy.test.Test.MyInfo mi, com.qq.taf.proxy.test.Test.MyInfoHolder mo)
    {
        return testMyInfo(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMyInfo(int id, com.qq.taf.proxy.test.Test.MyInfo mi, com.qq.taf.proxy.test.Test.MyInfoHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMyInfo", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new com.qq.taf.proxy.test.Test.MyInfo();
        mo.value = (com.qq.taf.proxy.test.Test.MyInfo) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMyInfo(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.MyInfo mi)
    {
        async_testMyInfo(callback, id, mi, __defaultContext());
    }

    public void async_testMyInfo(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.MyInfo mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMyInfo", _sBuffer, __ctx);

    }

    public int testTestInfo(int id, com.qq.taf.proxy.test.Test.TestInfo ti, com.qq.taf.proxy.test.Test.TestInfoHolder to)
    {
        return testTestInfo(id, ti, to, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testTestInfo(int id, com.qq.taf.proxy.test.Test.TestInfo ti, com.qq.taf.proxy.test.Test.TestInfoHolder to, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(ti, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testTestInfo", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        to.value = new com.qq.taf.proxy.test.Test.TestInfo();
        to.value = (com.qq.taf.proxy.test.Test.TestInfo) _is.read(to.value, 3, true);
        return _ret;
    }

    public void async_testTestInfo(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.TestInfo ti)
    {
        async_testTestInfo(callback, id, ti, __defaultContext());
    }

    public void async_testTestInfo(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.TestInfo ti, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(ti, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testTestInfo", _sBuffer, __ctx);

    }

    public int testVectorStruct(int id, com.qq.taf.proxy.test.Test.TestInfo[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorStruct(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorStruct(int id, com.qq.taf.proxy.test.Test.TestInfo[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorStruct", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (com.qq.taf.proxy.test.Test.TestInfo[]) new com.qq.taf.proxy.test.Test.TestInfo[1];
        com.qq.taf.proxy.test.Test.TestInfo __var_39 = new com.qq.taf.proxy.test.Test.TestInfo();
        ((com.qq.taf.proxy.test.Test.TestInfo[])vo.value)[0] = __var_39;
        vo.value = (com.qq.taf.proxy.test.Test.TestInfo[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorStruct(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.TestInfo[] vi)
    {
        async_testVectorStruct(callback, id, vi, __defaultContext());
    }

    public void async_testVectorStruct(com.qq.taf.proxy.test.Test.TestExtPrxCallback callback, int id, com.qq.taf.proxy.test.Test.TestInfo[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorStruct", _sBuffer, __ctx);

    }

}
