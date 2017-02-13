package com.qq.taf;

public abstract class PingFPrxCallback extends com.qq.taf.proxy.ServantProxyCallback
{
    protected String __taf__PingF_all[] = 
    {
        "tafPing",
    };

    protected String sServerEncoding = "GBK";
    public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }

    public abstract void callback_tafPing();

    public abstract void callback_tafPing_exception(int _iRet);

    final public int _onDispatch(String sFuncName, com.qq.taf.ResponsePacket response)
    {

        int iPos = java.util.Arrays.binarySearch(__taf__PingF_all, sFuncName);
        if(iPos < 0 || iPos >= 1) return -1; //or throw execption?
        switch(iPos)
        {
            case 0:
            {
                if ( response.iRet != com.qq.taf.cnst.JCESERVERSUCCESS.value )
                {
                	callback_tafPing_exception(response.iRet);
                    return response.iRet;
                }
                callback_tafPing();
                break;
            }
        }
        return 0;
    }
}
