// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.3 by WSRD Tencent.
// Generated from `SecretBase.jce'
// **********************************************************************

package com.duowan.QUAN;

public final class RemoveSecretReq extends com.duowan.taf.jce.JceStruct implements java.lang.Cloneable
{
    public String className()
    {
        return "QUAN.RemoveSecretReq";
    }

    public String fullClassName()
    {
        return "com.duowan.QUAN.RemoveSecretReq";
    }

    public com.duowan.QUAN.UserId tUserId = null;

    public long lUId = 0;

    public long lSId = 0;

    public com.duowan.QUAN.UserId getTUserId()
    {
        return tUserId;
    }

    public void  setTUserId(com.duowan.QUAN.UserId tUserId)
    {
        this.tUserId = tUserId;
    }

    public long getLUId()
    {
        return lUId;
    }

    public void  setLUId(long lUId)
    {
        this.lUId = lUId;
    }

    public long getLSId()
    {
        return lSId;
    }

    public void  setLSId(long lSId)
    {
        this.lSId = lSId;
    }

    public RemoveSecretReq()
    {
        setTUserId(tUserId);
        setLUId(lUId);
        setLSId(lSId);
    }

    public RemoveSecretReq(com.duowan.QUAN.UserId tUserId, long lUId, long lSId)
    {
        setTUserId(tUserId);
        setLUId(lUId);
        setLSId(lSId);
    }

    public boolean equals(Object o)
    {
        if(o == null)
        {
            return false;
        }

        RemoveSecretReq t = (RemoveSecretReq) o;
        return (
            com.duowan.taf.jce.JceUtil.equals(tUserId, t.tUserId) && 
            com.duowan.taf.jce.JceUtil.equals(lUId, t.lUId) && 
            com.duowan.taf.jce.JceUtil.equals(lSId, t.lSId) );
    }

    public int hashCode()
    {
        try
        {
            throw new Exception("Need define key first!");
        }
        catch(Exception ex)
        {
            ex.printStackTrace();
        }
        return 0;
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

    public void writeTo(com.duowan.taf.jce.JceOutputStream _os)
    {
        if (null != tUserId)
        {
            _os.write(tUserId, 0);
        }
        _os.write(lUId, 1);
        _os.write(lSId, 2);
    }

    static com.duowan.QUAN.UserId cache_tUserId;

    public void readFrom(com.duowan.taf.jce.JceInputStream _is)
    {
        if(null == cache_tUserId)
        {
            cache_tUserId = new com.duowan.QUAN.UserId();
        }
        setTUserId((com.duowan.QUAN.UserId) _is.read(cache_tUserId, 0, false));

        setLUId((long) _is.read(lUId, 1, false));

        setLSId((long) _is.read(lSId, 2, false));

    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.duowan.taf.jce.JceDisplayer _ds = new com.duowan.taf.jce.JceDisplayer(_os, _level);
        _ds.display(tUserId, "tUserId");
        _ds.display(lUId, "lUId");
        _ds.display(lSId, "lSId");
    }

}

