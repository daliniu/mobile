package com.qq.taf;

import com.qq.jce.wup.WupHexUtil;

public final class RequestPacket extends com.qq.taf.jce.JceStruct
{
    public short iVersion = 0;

    public byte cPacketType = 0;

    public int iMessageType = 0;

    public int iRequestId = 0;

    public String sServantName = null;

    public String sFuncName = null;

    public byte[] sBuffer;

    public int iTimeout = 0;

    public java.util.Map<String, String> context;

    public java.util.Map<String, String> status;

    public RequestPacket()
    {
    }

    public RequestPacket(short iVersion, byte cPacketType, int iMessageType, int iRequestId, String sServantName, String sFuncName, byte[] sBuffer, int iTimeout, java.util.Map<String, String> context, java.util.Map<String, String> status)
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
        _os.write(iMessageType, 3);
        _os.write(iRequestId, 4);
        _os.write(sServantName, 5);
        _os.write(sFuncName, 6);
        _os.write(sBuffer, 7);
        _os.write(iTimeout, 8);
        _os.write(context, 9);
        _os.write(status, 10);
        //System.out.println(sFuncName+" writeTo STATUS_SAMPLE_KEY:"+status.get("STATUS_SAMPLE_KEY"));
    }

    static byte[] cache_sBuffer = null ;
    static java.util.Map<String, String> cache_context = null ;
    
    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
    	try {
			iVersion = (short) _is.read(iVersion, 1, true);
			cPacketType = (byte) _is.read(cPacketType, 2, true);
			iMessageType = (int) _is.read(iMessageType, 3, true);
			iRequestId = (int) _is.read(iRequestId, 4, true);
			sServantName = (String) _is.readString( 5, true);
			sFuncName = (String) _is.readString(6, true);
			if ( null == cache_sBuffer  ) {
				cache_sBuffer = new byte[]{0};
			}
			sBuffer = (byte[]) _is.read(cache_sBuffer, 7, true);
			iTimeout = (int) _is.read(iTimeout, 8, true);
			if ( null == cache_context  ) {
				cache_context = new java.util.HashMap<String, String>();
				cache_context.put("", "");
			}
			context = (java.util.Map<String, String>) _is.read(cache_context, 9, true);
			if ( null == cache_context  ) {
				cache_context = new java.util.HashMap<String, String>();
				cache_context.put("", "");
			}
			status = (java.util.Map<String, String>) _is.read(cache_context, 10, true);
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("RequestPacket decode error "+WupHexUtil.bytes2HexStr(sBuffer));
			throw new RuntimeException(e);
		}


    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
        _ds.display(iVersion, "iVersion");
        _ds.display(cPacketType, "cPacketType");
        _ds.display(iMessageType, "iMessageType");
        _ds.display(iRequestId, "iRequestId");
        _ds.display(sServantName, "sServantName");
        _ds.display(sFuncName, "sFuncName");
        _ds.display(sBuffer, "sBuffer");
        _ds.display(iTimeout, "iTimeout");
        _ds.display(context, "context");
        _ds.display(status, "status");
    }

}
