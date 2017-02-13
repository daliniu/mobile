package com.duowan.taf.holder;

import java.util.ArrayList;

public final class JceArrayListHolder
{
    public
    JceArrayListHolder()
    {
    }

    public
    JceArrayListHolder(java.util.ArrayList value)
    {
        this.value = value;
    }

    public java.util.ArrayList value;

	public ArrayList getValue() {
		return value;
	}

	public void setValue(ArrayList value) {
		this.value = value;
		
	}
}
