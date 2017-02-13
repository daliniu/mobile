package com.qq.taf.proxy.test.Test;

public abstract class TestBasePrxCallback implements com.qq.taf.proxy.CallbackHandler
{
    protected String __Test__TestBase_all[] = 
    {
        "testMapByte",
        "testMapInt",
        "testMapIntDouble",
        "testMapIntFloat",
        "testMapIntStr",
        "testMapStr",
        "testStr",
        "testVectorByte",
        "testVectorDouble",
        "testVectorFloat",
        "testVectorInt",
        "testVectorShort",
        "testVectorStr"
    };

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public abstract void callback_testMapByte(int _ret, java.util.Map<Byte, String> mo);

    public abstract void callback_testMapByte_exception(int _iRet);

    public abstract void callback_testMapInt(int _ret, java.util.Map<Integer, Integer> mo);

    public abstract void callback_testMapInt_exception(int _iRet);

    public abstract void callback_testMapIntDouble(int _ret, java.util.Map<Integer, Double> mo);

    public abstract void callback_testMapIntDouble_exception(int _iRet);

    public abstract void callback_testMapIntFloat(int _ret, java.util.Map<Integer, Float> mo);

    public abstract void callback_testMapIntFloat_exception(int _iRet);

    public abstract void callback_testMapIntStr(int _ret, java.util.Map<Integer, String> mo);

    public abstract void callback_testMapIntStr_exception(int _iRet);

    public abstract void callback_testMapStr(int _ret, java.util.Map<String, String> mo);

    public abstract void callback_testMapStr_exception(int _iRet);

    public abstract void callback_testStr(int _ret, String so);

    public abstract void callback_testStr_exception(int _iRet);

    public abstract void callback_testVectorByte(int _ret, byte[] vo);

    public abstract void callback_testVectorByte_exception(int _iRet);

    public abstract void callback_testVectorDouble(int _ret, double[] vo);

    public abstract void callback_testVectorDouble_exception(int _iRet);

    public abstract void callback_testVectorFloat(int _ret, float[] vo);

    public abstract void callback_testVectorFloat_exception(int _iRet);

    public abstract void callback_testVectorInt(int _ret, int[] vo);

    public abstract void callback_testVectorInt_exception(int _iRet);

    public abstract void callback_testVectorShort(int _ret, short[] vo);

    public abstract void callback_testVectorShort_exception(int _iRet);

    public abstract void callback_testVectorStr(int _ret, String[] vo);

    public abstract void callback_testVectorStr_exception(int _iRet);

    final public int _onDispatch(String sFuncName, com.qq.taf.ResponsePacket response)
    {

        int iPos = java.util.Arrays.binarySearch(__Test__TestBase_all, sFuncName);
        if(iPos < 0 || iPos >= 13) return -1; //or throw execption?
        switch(iPos)
        {
            case 0:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapByte_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Byte, String> mo = new java.util.HashMap<Byte, String>();
                Byte __var_19 = 0;
                String __var_20 = "";
                mo.put(__var_19, __var_20);
                mo = (java.util.Map<Byte, String>) _is.read(mo, 3, true);

                callback_testMapByte(_ret, mo);
                break;
            }
            case 1:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapInt_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Integer, Integer> mo = new java.util.HashMap<Integer, Integer>();
                Integer __var_21 = 0;
                Integer __var_22 = 0;
                mo.put(__var_21, __var_22);
                mo = (java.util.Map<Integer, Integer>) _is.read(mo, 3, true);

                callback_testMapInt(_ret, mo);
                break;
            }
            case 2:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapIntDouble_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Integer, Double> mo = new java.util.HashMap<Integer, Double>();
                Integer __var_23 = 0;
                Double __var_24 = 0.0;
                mo.put(__var_23, __var_24);
                mo = (java.util.Map<Integer, Double>) _is.read(mo, 3, true);

                callback_testMapIntDouble(_ret, mo);
                break;
            }
            case 3:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapIntFloat_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Integer, Float> mo = new java.util.HashMap<Integer, Float>();
                Integer __var_25 = 0;
                Float __var_26 = 0.0f;
                mo.put(__var_25, __var_26);
                mo = (java.util.Map<Integer, Float>) _is.read(mo, 3, true);

                callback_testMapIntFloat(_ret, mo);
                break;
            }
            case 4:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapIntStr_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Integer, String> mo = new java.util.HashMap<Integer, String>();
                Integer __var_27 = 0;
                String __var_28 = "";
                mo.put(__var_27, __var_28);
                mo = (java.util.Map<Integer, String>) _is.read(mo, 3, true);

                callback_testMapIntStr(_ret, mo);
                break;
            }
            case 5:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapStr_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<String, String> mo = new java.util.HashMap<String, String>();
                String __var_29 = "";
                String __var_30 = "";
                mo.put(__var_29, __var_30);
                mo = (java.util.Map<String, String>) _is.read(mo, 3, true);

                callback_testMapStr(_ret, mo);
                break;
            }
            case 6:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testStr_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                String so = "";
                so = (String) _is.read(so, 3, true);

                callback_testStr(_ret, so);
                break;
            }
            case 7:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorByte_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                byte[] vo = (byte[]) new byte[1];
                Byte __var_31 = 0;
                ((byte[])vo)[0] = __var_31;
                vo = (byte[]) _is.read(vo, 3, true);

                callback_testVectorByte(_ret, vo);
                break;
            }
            case 8:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorDouble_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                double[] vo = (double[]) new double[1];
                Double __var_32 = 0.0;
                ((double[])vo)[0] = __var_32;
                vo = (double[]) _is.read(vo, 3, true);

                callback_testVectorDouble(_ret, vo);
                break;
            }
            case 9:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorFloat_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                float[] vo = (float[]) new float[1];
                Float __var_33 = 0.0f;
                ((float[])vo)[0] = __var_33;
                vo = (float[]) _is.read(vo, 3, true);

                callback_testVectorFloat(_ret, vo);
                break;
            }
            case 10:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorInt_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                int[] vo = (int[]) new int[1];
                Integer __var_34 = 0;
                ((int[])vo)[0] = __var_34;
                vo = (int[]) _is.read(vo, 3, true);

                callback_testVectorInt(_ret, vo);
                break;
            }
            case 11:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorShort_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                short[] vo = (short[]) new short[1];
                Short __var_35 = 0;
                ((short[])vo)[0] = __var_35;
                vo = (short[]) _is.read(vo, 3, true);

                callback_testVectorShort(_ret, vo);
                break;
            }
            case 12:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorStr_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                String[] vo = (String[]) new String[1];
                String __var_36 = "";
                ((String[])vo)[0] = __var_36;
                vo = (String[]) _is.read(vo, 3, true);

                callback_testVectorStr(_ret, vo);
                break;
            }
        }
        return 0;
    }
}
