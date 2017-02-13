package com.qq.taf.proxy.test.Test;

public final class MyInfo extends com.qq.taf.jce.JceStruct
{
    public int i = (int)0;

    public String s = (String)"";

    public MyInfo()
    {
    }

    public MyInfo(int i, String s)
    {
        this.i = i;
        this.s = s;
    }

    public int compareTo(MyInfo o)
    {
        int[] r = 
        {
            com.qq.taf.jce.JceUtil.compareTo(i, o.i), 
            com.qq.taf.jce.JceUtil.compareTo(s, o.s)
        };
        for(int i = 0; i < r.length; ++i)
        {
            if(r[i] != 0) return r[i];
        }
        return 0;
    }

    public boolean equals(Object o)
    {
        MyInfo t = (MyInfo) o;
        return (
            com.qq.taf.jce.JceUtil.equals(i, t.i) && 
            com.qq.taf.jce.JceUtil.equals(s, t.s) );
    }

    public int hashCode()
    {
        int [] hc = { 
            com.qq.taf.jce.JceUtil.hashCode(i), 
            com.qq.taf.jce.JceUtil.hashCode(s)
        };
        return java.util.Arrays.hashCode(hc);
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
        _os.write(i, 0);
        _os.write(s, 1);
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        i = 0;
        i = (int) _is.read(i, 0, true);

        s = "";
        s = (String) _is.read(s, 1, true);

    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
        _ds.display(i, "i");
        _ds.display(s, "s");
    }

}
