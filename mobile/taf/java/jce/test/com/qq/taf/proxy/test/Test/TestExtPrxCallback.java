package com.qq.taf.proxy.test.Test;

public abstract class TestExtPrxCallback implements com.qq.taf.proxy.CallbackHandler
{
    protected String __Test__TestExt_all[] = 
    {
        "testMapStruct",
        "testMyInfo",
        "testTestInfo",
        "testVectorStruct"
    };

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public abstract void callback_testMapStruct(int _ret, java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mo);

    public abstract void callback_testMapStruct_exception(int _iRet);

    public abstract void callback_testMyInfo(int _ret, com.qq.taf.proxy.test.Test.MyInfo mo);

    public abstract void callback_testMyInfo_exception(int _iRet);

    public abstract void callback_testTestInfo(int _ret, com.qq.taf.proxy.test.Test.TestInfo to);

    public abstract void callback_testTestInfo_exception(int _iRet);

    public abstract void callback_testVectorStruct(int _ret, com.qq.taf.proxy.test.Test.TestInfo[] vo);

    public abstract void callback_testVectorStruct_exception(int _iRet);

    final public int _onDispatch(String sFuncName, com.qq.taf.ResponsePacket response)
    {

        int iPos = java.util.Arrays.binarySearch(__Test__TestExt_all, sFuncName);
        if(iPos < 0 || iPos >= 4) return -1; //or throw execption?
        switch(iPos)
        {
            case 0:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMapStruct_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo> mo = new java.util.HashMap<Integer, com.qq.taf.proxy.test.Test.MyInfo>();
                Integer __var_40 = 0;
                com.qq.taf.proxy.test.Test.MyInfo __var_41 = new com.qq.taf.proxy.test.Test.MyInfo();
                mo.put(__var_40, __var_41);
                mo = (java.util.Map<Integer, com.qq.taf.proxy.test.Test.MyInfo>) _is.read(mo, 3, true);

                callback_testMapStruct(_ret, mo);
                break;
            }
            case 1:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testMyInfo_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                com.qq.taf.proxy.test.Test.MyInfo mo = new com.qq.taf.proxy.test.Test.MyInfo();
                mo = (com.qq.taf.proxy.test.Test.MyInfo) _is.read(mo, 3, true);

                callback_testMyInfo(_ret, mo);
                break;
            }
            case 2:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testTestInfo_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                com.qq.taf.proxy.test.Test.TestInfo to = new com.qq.taf.proxy.test.Test.TestInfo();
                to = (com.qq.taf.proxy.test.Test.TestInfo) _is.read(to, 3, true);

                callback_testTestInfo(_ret, to);
                break;
            }
            case 3:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testVectorStruct_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                com.qq.taf.proxy.test.Test.TestInfo[] vo = (com.qq.taf.proxy.test.Test.TestInfo[]) new com.qq.taf.proxy.test.Test.TestInfo[1];
                com.qq.taf.proxy.test.Test.TestInfo __var_42 = new com.qq.taf.proxy.test.Test.TestInfo();
                ((com.qq.taf.proxy.test.Test.TestInfo[])vo)[0] = __var_42;
                vo = (com.qq.taf.proxy.test.Test.TestInfo[]) _is.read(vo, 3, true);

                callback_testVectorStruct(_ret, vo);
                break;
            }
        }
        return 0;
    }
}
