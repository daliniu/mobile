package com.qq.taf.holder;

public final class JceHolder<T>
{
    public
    JceHolder()
    {
    }

    public
    JceHolder(T value)
    {
        this.value = value;
    }

    public T value;
}
