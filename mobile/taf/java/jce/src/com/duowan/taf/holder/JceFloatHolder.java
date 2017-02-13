package com.duowan.taf.holder;


public final class JceFloatHolder 
{
    public
    JceFloatHolder()
    {
    }

    public
    JceFloatHolder(float value)
    {
        this.value = value;
    }

    public float value;

	public Float getValue() {
		return value;
	}

	public void setValue(Float value) {
		this.value = value;
		
	}
}
