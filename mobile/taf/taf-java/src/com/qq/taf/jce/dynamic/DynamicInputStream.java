package com.qq.taf.jce.dynamic;

import java.io.UnsupportedEncodingException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import com.qq.taf.jce.JceDecodeException;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceStruct;

public final class DynamicInputStream {
	
	private String sServerEncoding = "GBK";
    
	private ByteBuffer bs;
	
	public DynamicInputStream(ByteBuffer bs) {
		this.bs = bs;
	}

	public DynamicInputStream(byte[] bs) {
		this.bs = ByteBuffer.wrap(bs);
	}
	
	public int setServerEncoding(String se){
        sServerEncoding = se;
        return 0;
    }
	
	public JceField read() {
		try {
			JceInputStream.HeadData hd = new JceInputStream.HeadData();
			JceInputStream.readHead(hd, bs);
			switch (hd.type) {
			case JceStruct.BYTE:
				return JceField.create(bs.get(), hd.tag);
			case JceStruct.SHORT:
				return JceField.create(bs.getShort(), hd.tag);
			case JceStruct.INT:
				return JceField.create(bs.getInt(), hd.tag);
			case JceStruct.LONG:
				return JceField.create(bs.getLong(), hd.tag);
			case JceStruct.FLOAT:
				return JceField.create(bs.getFloat(), hd.tag);
			case JceStruct.DOUBLE:
				return JceField.create(bs.getDouble(), hd.tag);
			case JceStruct.STRING1:
			{
				int len = bs.get();
				if (len < 0)
					len += 256;
				return readString(hd, len);
			}
			case JceStruct.STRING4:
				return readString(hd, bs.getInt());
			case JceStruct.LIST:
			{
				int len = ((NumberField) read()).intValue();
				JceField[] fs = new JceField[len];
				for (int i = 0; i < len; ++i)
					fs[i] = read();
				return JceField.createList(fs, hd.tag);
			}
			case JceStruct.MAP:
			{
				int len = ((NumberField) read()).intValue();
				JceField[] keys = new JceField[len];
				JceField[] values = new JceField[len];
				for (int i = 0; i < len; ++i) {
					keys[i] = read();
					values[i] = read();
				}
				return JceField.createMap(keys, values, hd.tag);
			}
			case JceStruct.STRUCT_BEGIN:
			{
				List<JceField> ls = new ArrayList<JceField>();
				while (true) {
					JceField f = read();
					if (f == null)
						break;
					ls.add(f);
				}
				return JceField.createStruct(ls.toArray(new JceField[0]), hd.tag);
			}
			case JceStruct.STRUCT_END:
				return null;
			case JceStruct.ZERO_TAG:
				return JceField.createZero(hd.tag);
			case JceStruct.SIMPLE_LIST:
			{
				int tag = hd.tag;
				JceInputStream.readHead(hd, bs);
				if (hd.type != JceStruct.BYTE)
					throw new JceDecodeException("type mismatch, simple_list only support byte, tag: " + tag + ", type: " + hd.type);
				int len = ((NumberField) read()).intValue();
				byte[] data = new byte[len];
				bs.get(data);
				return JceField.create(data, tag);
			}
			}
		} catch (BufferUnderflowException e) {
			return null;
		}
		return null;
	}

	private JceField readString(JceInputStream.HeadData hd, int len) {
		byte[] ss = new byte[len];
		bs.get(ss);
		String s = null;
		try{
			s = new String(ss, sServerEncoding);
		}
		catch (UnsupportedEncodingException e){
			s = new String(ss);
		}
		return JceField.create(s, hd.tag);
	}
}
