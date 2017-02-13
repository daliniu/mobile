package com.qq.taf.proxy.test.Test;

public final class TestInfo extends com.qq.taf.jce.JceStruct
{
    public boolean b = (boolean)true;

    public byte by = (byte)0;

    public short si = (short)0;

    public int ii = (int)0;

    public long li = (long)0;

    public float f = (float)0;

    public double d = (double)0;

    public String s = (String)"";

    public com.qq.taf.proxy.test.Test.MyInfo[] vt;

    public String[] vs;

    public java.util.Map<String, String> m;

    public java.util.Map<String, String>[] vm;

    public java.util.Map<String[], String[]> mv;

    public TestInfo()
    {
    }

    public TestInfo(boolean b, byte by, short si, int ii, long li, float f, double d, String s, com.qq.taf.proxy.test.Test.MyInfo[] vt, String[] vs, java.util.Map<String, String> m, java.util.Map<String, String>[] vm, java.util.Map<String[], String[]> mv)
    {
        this.b = b;
        this.by = by;
        this.si = si;
        this.ii = ii;
        this.li = li;
        this.f = f;
        this.d = d;
        this.s = s;
        this.vt = vt;
        this.vs = vs;
        this.m = m;
        this.vm = vm;
        this.mv = mv;
    }

    public int compareTo(TestInfo o)
    {
        int[] r = 
        {
            com.qq.taf.jce.JceUtil.compareTo(li, o.li), 
            com.qq.taf.jce.JceUtil.compareTo(ii, o.ii), 
            com.qq.taf.jce.JceUtil.compareTo(b, o.b)
        };
        for(int i = 0; i < r.length; ++i)
        {
            if(r[i] != 0) return r[i];
        }
        return 0;
    }

    public boolean equals(Object o)
    {
        TestInfo t = (TestInfo) o;
        return (
            com.qq.taf.jce.JceUtil.equals(li, t.li) && 
            com.qq.taf.jce.JceUtil.equals(ii, t.ii) && 
            com.qq.taf.jce.JceUtil.equals(b, t.b) );
    }

    public int hashCode()
    {
        int [] hc = { 
            com.qq.taf.jce.JceUtil.hashCode(li), 
            com.qq.taf.jce.JceUtil.hashCode(ii), 
            com.qq.taf.jce.JceUtil.hashCode(b)
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
        _os.write(b, 0);
        _os.write(by, 1);
        _os.write(si, 2);
        _os.write(ii, 3);
        _os.write(li, 4);
        _os.write(f, 5);
        _os.write(d, 6);
        _os.write(s, 7);
        _os.write(vt, 8);
        _os.write(vs, 9);
        _os.write(m, 10);
        _os.write(vm, 11);
        _os.write(mv, 12);
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        b = false;
        b = (boolean) _is.read(b, 0, true);

        by = 0;
        by = (byte) _is.read(by, 1, true);

        si = 0;
        si = (short) _is.read(si, 2, true);

        ii = 0;
        ii = (int) _is.read(ii, 3, true);

        li = 0;
        li = (long) _is.read(li, 4, true);

        f = 0.0f;
        f = (float) _is.read(f, 5, true);

        d = 0.0;
        d = (double) _is.read(d, 6, true);

        s = "";
        s = (String) _is.read(s, 7, true);

        vt = (com.qq.taf.proxy.test.Test.MyInfo[]) new com.qq.taf.proxy.test.Test.MyInfo[1];
        com.qq.taf.proxy.test.Test.MyInfo __var_43 = new com.qq.taf.proxy.test.Test.MyInfo();
        ((com.qq.taf.proxy.test.Test.MyInfo[])vt)[0] = __var_43;
        vt = (com.qq.taf.proxy.test.Test.MyInfo[]) _is.read(vt, 8, true);

        vs = (String[]) new String[1];
        String __var_44 = "";
        ((String[])vs)[0] = __var_44;
        vs = (String[]) _is.read(vs, 9, true);

        m = new java.util.HashMap<String, String>();
        String __var_45 = "";
        String __var_46 = "";
        m.put(__var_45, __var_46);
        m = (java.util.Map<String, String>) _is.read(m, 10, true);

        vm = (java.util.Map<String, String>[]) new java.util.Map[1];
        java.util.Map<String, String> __var_47 = new java.util.HashMap<String, String>();
        String __var_48 = "";
        String __var_49 = "";
        __var_47.put(__var_48, __var_49);
        ((java.util.Map<String, String>[])vm)[0] = __var_47;
        vm = (java.util.Map<String, String>[]) _is.read(vm, 11, true);

        mv = new java.util.HashMap<String[], String[]>();
        String[] __var_50 = (String[]) new String[1];
        String __var_51 = "";
        ((String[])__var_50)[0] = __var_51;
        String[] __var_52 = (String[]) new String[1];
        String __var_53 = "";
        ((String[])__var_52)[0] = __var_53;
        mv.put(__var_50, __var_52);
        mv = (java.util.Map<String[], String[]>) _is.read(mv, 12, true);

    }

    public void display(java.lang.StringBuilder _os, int _level)
    {
        com.qq.taf.jce.JceDisplayer _ds = new com.qq.taf.jce.JceDisplayer(_os, _level);
        _ds.display(b, "b");
        _ds.display(by, "by");
        _ds.display(si, "si");
        _ds.display(ii, "ii");
        _ds.display(li, "li");
        _ds.display(f, "f");
        _ds.display(d, "d");
        _ds.display(s, "s");
        _ds.display(vt, "vt");
        _ds.display(vs, "vs");
        _ds.display(m, "m");
        _ds.display(vm, "vm");
        _ds.display(mv, "mv");
    }

}
