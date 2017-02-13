package com.qq.taf;

public final class ResponsePacket extends com.qq.taf.jce.JceStruct
{
    public short iVersion = (short)0;

    public byte cPacketType = (byte)0;

    public int iRequestId = (int)0;

    public int iMessageType = (int)0;

    public int iRet = (int)0;

    public byte[] sBuffer;

    public java.util.Map<String, String> status;

    public ResponsePacket()
    {
    }

    public ResponsePacket(short iVersion, byte cPacketType, int iRequestId, int iMessageType, int iRet, byte[] sBuffer, java.util.Map<String, String> status)
    {
        this.iVersion = iVersion;
        this.cPacketType = cPacketType;
        this.iRequestId = iRequestId;
        this.iMessageType = iMessageType;
        this.iRet = iRet;
        this.sBuffer = sBuffer;
        this.status = status;
    }

    public boolean equals(Object o)
    {
        ResponsePacket t = (ResponsePacket) o;
        return (
            com.qq.taf.jce.JceUtil.equals(1, t.iVersion) && 
            com.qq.taf.jce.JceUtil.equals(1, t.cPacketType) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iRequestId) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iMessageType) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iRet) && 
            com.qq.taf.jce.JceUtil.equals(1, t.sBuffer) && 
            com.qq.taf.jce.JceUtil.equals(1, t.status) );
    }

    public java.lang.Object clone()
    {
        java.lang.Object o = null;
        try
        {
            o = super.clone();
        }
        catch(CloneNotSupportedException ex)
        {
            assert false; // impossible
        }
        return o;
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        _os.write(iVersion, 1);
        _os.write(cPacketType, 2);
        _os.write(iRequestId, 3);
        _os.write(iMessageType, 4);
        _os.write(iRet, 5);
        _os.write(sBuffer, 6);
        _os.write(status, 7);
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        iVersion = 0;
        iVersion = (short) _is.read(iVersion, 1, true);

        cPacketType = 0;
        cPacketType = (byte) _is.read(cPacketType, 2, true);

        iRequestId = 0;
        iRequestId = (int) _is.read(iRequestId, 3, true);

        iMessageType = 0;
        iMessageType = (int) _is.read(iMessageType, 4, true);

        iRet = 0;
        iRet = (int) _is.read(iRet, 5, true);

        sBuffer = (byte[]) new byte[1];
        Byte __var_6 = 0;
        ((byte[])sBuffer)[0] = __var_6;
        sBuffer = (byte[]) _is.read(sBuffer, 6, true);

        status = new java.util.HashMap<String, String>();
        String __var_7 = "";
        String __var_8 = "";
        status.put(__var_7, __var_8);
        status = (java.util.Map<String, String>) _is.read(status, 7, true);

    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
        _ds.display(iVersion, "iVersion");
        _ds.display(cPacketType, "cPacketType");
        _ds.display(iRequestId, "iRequestId");
        _ds.display(iMessageType, "iMessageType");
        _ds.display(iRet, "iRet");
        _ds.display(sBuffer, "sBuffer");
        _ds.display(status, "status");
    }

}
