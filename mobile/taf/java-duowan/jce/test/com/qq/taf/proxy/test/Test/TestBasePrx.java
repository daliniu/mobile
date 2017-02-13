package com.qq.taf.proxy.test.Test;

public final class TestBasePrx extends com.qq.taf.proxy.ServantProxy
{

    public TestBasePrx taf_hash(int hashCode)
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

    public int testMapByte(int id, java.util.Map<Byte, String> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapByte(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapByte(int id, java.util.Map<Byte, String> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapByte", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Byte, String>();
        Byte __var_1 = 0;
        String __var_2 = "";
        mo.value.put(__var_1, __var_2);
        mo.value = (java.util.Map<Byte, String>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapByte(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Byte, String> mi)
    {
        async_testMapByte(callback, id, mi, __defaultContext());
    }

    public void async_testMapByte(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Byte, String> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapByte", _sBuffer, __ctx);

    }

    public int testMapInt(int id, java.util.Map<Integer, Integer> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapInt(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapInt(int id, java.util.Map<Integer, Integer> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapInt", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Integer, Integer>();
        Integer __var_3 = 0;
        Integer __var_4 = 0;
        mo.value.put(__var_3, __var_4);
        mo.value = (java.util.Map<Integer, Integer>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapInt(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Integer> mi)
    {
        async_testMapInt(callback, id, mi, __defaultContext());
    }

    public void async_testMapInt(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Integer> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapInt", _sBuffer, __ctx);

    }

    public int testMapIntDouble(int id, java.util.Map<Integer, Double> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapIntDouble(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapIntDouble(int id, java.util.Map<Integer, Double> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapIntDouble", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Integer, Double>();
        Integer __var_5 = 0;
        Double __var_6 = 0.0;
        mo.value.put(__var_5, __var_6);
        mo.value = (java.util.Map<Integer, Double>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapIntDouble(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Double> mi)
    {
        async_testMapIntDouble(callback, id, mi, __defaultContext());
    }

    public void async_testMapIntDouble(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Double> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapIntDouble", _sBuffer, __ctx);

    }

    public int testMapIntFloat(int id, java.util.Map<Integer, Float> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapIntFloat(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapIntFloat(int id, java.util.Map<Integer, Float> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapIntFloat", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Integer, Float>();
        Integer __var_7 = 0;
        Float __var_8 = 0.0f;
        mo.value.put(__var_7, __var_8);
        mo.value = (java.util.Map<Integer, Float>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapIntFloat(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Float> mi)
    {
        async_testMapIntFloat(callback, id, mi, __defaultContext());
    }

    public void async_testMapIntFloat(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, Float> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapIntFloat", _sBuffer, __ctx);

    }

    public int testMapIntStr(int id, java.util.Map<Integer, String> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapIntStr(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapIntStr(int id, java.util.Map<Integer, String> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapIntStr", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<Integer, String>();
        Integer __var_9 = 0;
        String __var_10 = "";
        mo.value.put(__var_9, __var_10);
        mo.value = (java.util.Map<Integer, String>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapIntStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, String> mi)
    {
        async_testMapIntStr(callback, id, mi, __defaultContext());
    }

    public void async_testMapIntStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<Integer, String> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapIntStr", _sBuffer, __ctx);

    }

    public int testMapStr(int id, java.util.Map<String, String> mi, com.qq.taf.holder.JceHashMapHolder mo)
    {
        return testMapStr(id, mi, mo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testMapStr(int id, java.util.Map<String, String> mi, com.qq.taf.holder.JceHashMapHolder mo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testMapStr", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        mo.value = new java.util.HashMap<String, String>();
        String __var_11 = "";
        String __var_12 = "";
        mo.value.put(__var_11, __var_12);
        mo.value = (java.util.Map<String, String>) _is.read(mo.value, 3, true);
        return _ret;
    }

    public void async_testMapStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<String, String> mi)
    {
        async_testMapStr(callback, id, mi, __defaultContext());
    }

    public void async_testMapStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, java.util.Map<String, String> mi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(mi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testMapStr", _sBuffer, __ctx);

    }

    public int testStr(int id, String si, com.qq.taf.holder.JceStringHolder so)
    {
        return testStr(id, si, so, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testStr(int id, String si, com.qq.taf.holder.JceStringHolder so, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(si, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testStr", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        so.value = "";
        so.value = (String) _is.read(so.value, 3, true);
        return _ret;
    }

    public void async_testStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, String si)
    {
        async_testStr(callback, id, si, __defaultContext());
    }

    public void async_testStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, String si, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(si, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testStr", _sBuffer, __ctx);

    }

    public int testVectorByte(int id, byte[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorByte(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorByte(int id, byte[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorByte", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (byte[]) new byte[1];
        Byte __var_13 = 0;
        ((byte[])vo.value)[0] = __var_13;
        vo.value = (byte[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorByte(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, byte[] vi)
    {
        async_testVectorByte(callback, id, vi, __defaultContext());
    }

    public void async_testVectorByte(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, byte[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorByte", _sBuffer, __ctx);

    }

    public int testVectorDouble(int id, double[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorDouble(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorDouble(int id, double[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorDouble", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (double[]) new double[1];
        Double __var_14 = 0.0;
        ((double[])vo.value)[0] = __var_14;
        vo.value = (double[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorDouble(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, double[] vi)
    {
        async_testVectorDouble(callback, id, vi, __defaultContext());
    }

    public void async_testVectorDouble(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, double[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorDouble", _sBuffer, __ctx);

    }

    public int testVectorFloat(int id, float[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorFloat(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorFloat(int id, float[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorFloat", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (float[]) new float[1];
        Float __var_15 = 0.0f;
        ((float[])vo.value)[0] = __var_15;
        vo.value = (float[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorFloat(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, float[] vi)
    {
        async_testVectorFloat(callback, id, vi, __defaultContext());
    }

    public void async_testVectorFloat(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, float[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorFloat", _sBuffer, __ctx);

    }

    public int testVectorInt(int id, int[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorInt(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorInt(int id, int[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorInt", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (int[]) new int[1];
        Integer __var_16 = 0;
        ((int[])vo.value)[0] = __var_16;
        vo.value = (int[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorInt(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, int[] vi)
    {
        async_testVectorInt(callback, id, vi, __defaultContext());
    }

    public void async_testVectorInt(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, int[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorInt", _sBuffer, __ctx);

    }

    public int testVectorShort(int id, short[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorShort(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorShort(int id, short[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorShort", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (short[]) new short[1];
        Short __var_17 = 0;
        ((short[])vo.value)[0] = __var_17;
        vo.value = (short[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorShort(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, short[] vi)
    {
        async_testVectorShort(callback, id, vi, __defaultContext());
    }

    public void async_testVectorShort(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, short[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorShort", _sBuffer, __ctx);

    }

    public int testVectorStr(int id, String[] vi, com.qq.taf.holder.JceArrayHolder vo)
    {
        return testVectorStr(id, vi, vo, __defaultContext());
    }

    @SuppressWarnings("unchecked")
    public int testVectorStr(int id, String[] vi, com.qq.taf.holder.JceArrayHolder vo, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        byte[] _returnSBuffer = taf_invoke("testVectorStr", _sBuffer, __ctx);

        com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(_returnSBuffer);
        _is.setServerEncoding(sServerEncoding);
        int _ret = 0;
        _ret = (int) _is.read(_ret, 0, true);
        vo.value = (String[]) new String[1];
        String __var_18 = "";
        ((String[])vo.value)[0] = __var_18;
        vo.value = (String[]) _is.read(vo.value, 3, true);
        return _ret;
    }

    public void async_testVectorStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, String[] vi)
    {
        async_testVectorStr(callback, id, vi, __defaultContext());
    }

    public void async_testVectorStr(com.qq.taf.proxy.test.Test.TestBasePrxCallback callback, int id, String[] vi, java.util.Map __ctx)
    {
        com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
        _os.setServerEncoding(sServerEncoding);
        _os.write(id, 1);
        _os.write(vi, 2);
        byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());

        taf_invokeAsync(callback, "testVectorStr", _sBuffer, __ctx);

    }

}
