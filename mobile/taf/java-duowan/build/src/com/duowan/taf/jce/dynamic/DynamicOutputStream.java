package com.duowan.taf.jce.dynamic;

import java.nio.ByteBuffer;

import com.duowan.taf.jce.JceDecodeException;
import com.duowan.taf.jce.JceOutputStream;
import com.duowan.taf.jce.JceStruct;

public final class DynamicOutputStream extends JceOutputStream {
	public DynamicOutputStream(ByteBuffer bs) {
		super(bs);
	}

	public DynamicOutputStream(int capacity) {
		super(capacity);
	}

	public DynamicOutputStream() {
		super();
	}

	public void write(JceField field) {
		final int tag = field.getTag();
		if (field instanceof ZeroField) {
			write(0, tag);
		} else if (field instanceof IntField) {
			write(((IntField) field).get(), tag);
		} else if (field instanceof ShortField) {
			write(((ShortField) field).get(), tag);
		} else if (field instanceof ByteField) {
			write(((ByteField) field).get(), tag);
		} else if (field instanceof StringField) {
			write(((StringField) field).get(), tag);
		} else if (field instanceof ByteArrayField) {
			write(((ByteArrayField) field).get(), tag);
		} else if (field instanceof ListField) {
			ListField lf = (ListField) field;
			reserve(8);
			writeHead(JceStruct.LIST, tag);
			write(lf.size(), 0);
			for (JceField jf : lf.get())
				write(jf);
		} else if (field instanceof MapField) {
			MapField mf = (MapField) field;
			reserve(8);
			writeHead(JceStruct.MAP, tag);
			int ns = mf.size();
			write(ns, 0);
			for (int i = 0; i < ns; ++i) {
				write(mf.getKey(i));
				write(mf.getValue(i));
			}
		} else if (field instanceof StructField) {
			StructField sf = (StructField) field;
			reserve(2);
			writeHead(JceStruct.STRUCT_BEGIN, tag);
			for (JceField jf : sf.get())
				write(jf);
			reserve(2);
			writeHead(JceStruct.STRUCT_END, 0);
		} else if (field instanceof LongField) {
			write(((LongField) field).get(), tag);
		} else if (field instanceof FloatField) {
			write(((FloatField) field).get(), tag);
		} else if (field instanceof DoubleField) {
			write(((DoubleField) field).get(), tag);
		} else {
			throw new JceDecodeException("unknow JceField type: " + field.getClass().getName());
		}
	}
}
