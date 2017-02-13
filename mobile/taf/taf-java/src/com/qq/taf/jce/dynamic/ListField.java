package com.qq.taf.jce.dynamic;

public final class ListField extends JceField {
	
	private JceField[] data;

	ListField(JceField[] data, int tag) {
		super(tag);
		this.data = data;
	}

	public JceField[] get() {
		return data;
	}
	
	public JceField get(int n) {
		return data[n];
	}
	
	public void set(int n, JceField field) {
		data[n] = field;
	}
	
	public void set(JceField[] fields) {
		data = fields;
	}
	
	public int size() {
		return data.length;
	}
}
