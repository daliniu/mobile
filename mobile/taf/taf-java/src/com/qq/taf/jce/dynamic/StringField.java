package com.qq.taf.jce.dynamic;

public class StringField extends JceField {
	
	private String data;

	StringField(String data, int tag) {
		super(tag);
		this.data = data;
	}

	public String get() {
		return data;
	}
	
	public void set(String s) {
		data = s;
	}
}
