package com.qq.taf;

import java.io.IOException;

public final class RequestPacket extends com.qq.taf.jce.JceStruct
{
    public short iVersion = (short)0;

    public byte cPacketType = (byte)0;

    public int iMessageType = (int)0;

    public int iRequestId = (int)0;

    public String sServantName = (String)"";

    public String sFuncName = (String)"";

    public byte[] sBuffer;

    public int iTimeout = (int)0;

    public java.util.Hashtable context;

    public java.util.Hashtable status;

    public RequestPacket()
    {
    }

    public RequestPacket(short iVersion, byte cPacketType, int iMessageType, int iRequestId, String sServantName, String sFuncName, byte[] sBuffer, int iTimeout, java.util.Hashtable context, java.util.Hashtable status)
    {
        this.iVersion = iVersion;
        this.cPacketType = cPacketType;
        this.iMessageType = iMessageType;
        this.iRequestId = iRequestId;
        this.sServantName = sServantName;
        this.sFuncName = sFuncName;
        this.sBuffer = sBuffer;
        this.iTimeout = iTimeout;
        this.context = context;
        this.status = status;
    }

    public boolean equals(Object o)
    {
        RequestPacket t = (RequestPacket) o;
        return (
            com.qq.taf.jce.JceUtil.equals(1, t.iVersion) && 
            com.qq.taf.jce.JceUtil.equals(1, t.cPacketType) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iMessageType) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iRequestId) && 
            com.qq.taf.jce.JceUtil.equals(1, t.sServantName) && 
            com.qq.taf.jce.JceUtil.equals(1, t.sFuncName) && 
            com.qq.taf.jce.JceUtil.equals(1, t.sBuffer) && 
            com.qq.taf.jce.JceUtil.equals(1, t.iTimeout) && 
            com.qq.taf.jce.JceUtil.equals(1, t.context) && 
            com.qq.taf.jce.JceUtil.equals(1, t.status) );
    }

   

    public void writeTo(com.qq.taf.jce.JceOutputStream _os) throws IOException
    {
        _os.write(iVersion, 1);
        _os.write(cPacketType, 2);
        _os.write(iMessageType, 3);
        _os.write(iRequestId, 4);
        _os.write(sServantName, 5);
        _os.write(sFuncName, 6);
        _os.write(sBuffer, 7);
        _os.write(iTimeout, 8);
        _os.write(context, 9);
        _os.write(status, 10);
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is) throws IOException
    {
        iVersion = 0;
        iVersion = (short) _is.read(iVersion, 1, true);

        cPacketType = 0;
        cPacketType = (byte) _is.read(cPacketType, 2, true);

        iMessageType = 0;
        iMessageType = (int) _is.read(iMessageType, 3, true);

        iRequestId = 0;
        iRequestId = (int) _is.read(iRequestId, 4, true);

        sServantName = "";
        sServantName = (String) _is.read(sServantName, 5, true);

        sFuncName = "";
        sFuncName = (String) _is.read(sFuncName, 6, true);

        sBuffer = (byte[]) new byte[1];
        byte __var_1 = 0;
        ((byte[])sBuffer)[0] = __var_1;
        sBuffer = (byte[]) _is.read(sBuffer, 7, true);

        iTimeout = 0;
        iTimeout = (int) _is.read(iTimeout, 8, true);

        context = new java.util.Hashtable();
        String __var_2 = "";
        String __var_3 = "";
        context.put(__var_2, __var_3);
        context = (java.util.Hashtable) _is.read(context, 9, true);

        status = new java.util.Hashtable();
        String __var_4 = "";
        String __var_5 = "";
        status.put(__var_4, __var_5);
        status = (java.util.Hashtable) _is.read(status, 10, true);

    }

    public void display(java.lang.StringBuffer _os, int _level)
    {
//        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
//        _ds.display(iVersion, "iVersion");
//        _ds.display(cPacketType, "cPacketType");
//        _ds.display(iMessageType, "iMessageType");
//        _ds.display(iRequestId, "iRequestId");
//        _ds.display(sServantName, "sServantName");
//        _ds.display(sFuncName, "sFuncName");
//        _ds.display(sBuffer, "sBuffer");
//        _ds.display(iTimeout, "iTimeout");
//        _ds.display(context, "context");
//        _ds.display(status, "status");
    }

	public String className() {
		return "com.qq.taf.RequestPacket";
	}

}
