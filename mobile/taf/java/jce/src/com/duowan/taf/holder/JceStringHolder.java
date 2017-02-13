package com.duowan.taf.holder;

public final class JceStringHolder
{
    public
    JceStringHolder()
    {
    }

    public
    JceStringHolder(String t)
    {
        this.value = t;
    }

    public String value;

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
		
	}

}
