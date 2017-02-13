package com.qq.taf.proxy.test.router.Test;

public abstract class HelloPrxCallback implements com.qq.taf.proxy.CallbackHandler
{
    protected String __Test__Hello_all[] = 
    {
        "testHello"
    };

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public abstract void callback_testHello(int _ret, String r);

    public abstract void callback_testHello_exception(int _iRet);

    final public int _onDispatch(String sFuncName, com.qq.taf.ResponsePacket response)
    {

        int iPos = java.util.Arrays.binarySearch(__Test__Hello_all, sFuncName);
        if(iPos < 0 || iPos >= 1) return -1; //or throw execption?
        switch(iPos)
        {
            case 0:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                    callback_testHello_exception(response.iRet);
                    return response.iRet;
                }
                com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(response.sBuffer);
                _is.setServerEncoding(sServerEncoding);
                int _ret = 0;
                _ret = (int) _is.read(_ret, 0, true);

                String r = "";
                r = (String) _is.read(r, 2, true);

                callback_testHello(_ret, r);
                break;
            }
        }
        return 0;
    }
}
