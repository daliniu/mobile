package com.qq.taf.jce.dynamic;

public final class MapField extends JceField {
	
	private JceField[] keys;
	
	private JceField[] values;

	MapField(JceField[] keys, JceField[] values, int tag) {
		super(tag);
		this.keys = keys;
		this.values = values;
	}

	public JceField[] getKeys() {
		return keys;
	}
	
	public JceField[] getValues() {
		return values;
	}
	
	public int size() {
		return keys.length;
	}
	
	public JceField getKey(int n) {
		return keys[n];
	}
	
	public JceField getValue(int n) {
		return values[n];
	}
	
	public void setKey(int n, JceField field) {
		keys[n] = field;
	}
	
	public void setValue(int n, JceField field) {
		values[n] = field;
	}
}
