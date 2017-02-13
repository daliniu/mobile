package com.qq.jce.wup;

import java.util.Hashtable;

import com.qq.taf.jce.JceDisplayer;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.jce.JceStruct;
import com.qq.taf.jce.JceUtil;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;

public class UniAttribute extends JceStruct {

	protected Hashtable _data = new Hashtable();

	protected String encodeName = "utf-8";

	/**
	 * 返回编码字符集 默认为UTF-8
	 * 
	 * @return
	 */
	public String getEncodeName() {
		return encodeName;
	}

	/**
	 * 设置编码字符集 默认为GBK
	 * 
	 * @param encodeName
	 */
	public void setEncodeName(String encodeName) {
		this.encodeName = encodeName;
	}

	/**
	 * 返回放入数据结构中的所有key
	 * 
	 * @return
	 */
	public Enumeration getKeySet() {
		return _data.keys();
	}

	public boolean isEmpty() {
		return _data.isEmpty();
	}

	public int size() {
		return _data.size();
	}

	public boolean containsKey(String key) {
		return _data.containsKey(key);
	}

	/**
	 * 放入一个元素
	 * 
	 * @param <T>
	 * @param name
	 * @param t
	 */
	public void put(String name, Object value) throws IOException {
		if (name == null) {
			throw new IllegalArgumentException("put key can not is null");
		}
		if (value == null) {
			throw new IllegalArgumentException("put value can not is null");
		}
		JceOutputStream _out = new JceOutputStream();
		_out.setServerEncoding(encodeName);
		_out.write(value, 0);
		byte[] _sBuffer = _out.getFlushBytes();
		Hashtable pair = new Hashtable();
		Vector listTpye = new Vector();
		// HashMap<String, byte[]> pair = new HashMap<String, byte[]>();
		// ArrayList<String> listTpye = new ArrayList<String>();
		checkObjectType(listTpye, value);
		String className = BasicClassTypeUtil.transTypeList(listTpye);
		pair.put(className, _sBuffer);
		//System.out.println("-------put object name:"+name+" className:"+className+" sbuffer:"+WupHexUtil.bytes2HexStr(_sBuffer));
		_data.put(name, pair);
	}

	/**
	 * 获取一个元素
	 * 
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public Object get(Object proxy , String name) throws ObjectCreateException {
		if (!_data.containsKey(name)) {
			return null;
		} else {
			Hashtable pair = (Hashtable) _data.get(name);
			String className = "";
			byte[] data = new byte[0];
			Enumeration keys = pair.keys();
			while (keys.hasMoreElements()) {
				className = (String) keys.nextElement();
				data = (byte[]) pair.get(className);
				break;
			}
			try {
				//Object proxy = (Object) BasicClassTypeUtil.createClassByUni(className);
				JceInputStream _is = new JceInputStream(data);
				_is.setServerEncoding(encodeName);
				// System.out.println("try to read proxy "+proxy.getClass().getName()+" name is "+className+" data:"+WupHexUtil.bytes2HexStr(data));
				return (Object) _is.read(proxy, 0, true);
			} catch (Exception ex) {
				ex.printStackTrace();
				throw new ObjectCreateException(ex);
			}
		}
	}

	
	public void remove(String name) throws ObjectCreateException {
		if (_data.containsKey(name)) {
			_data.remove(name);
		} 
	}
	/**
	 * 删除一个元素
	 * 
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public Object remove(Object proxy ,String name) throws ObjectCreateException {
		if (!_data.containsKey(name)) {
			return null;
		} else {
			Hashtable pair = (Hashtable) _data.remove(name);
			String className = "";
			byte[] data = new byte[0];
			Enumeration keys = pair.keys();
			while (keys.hasMoreElements()) {
				className = (String) keys.nextElement();
				data = (byte[]) pair.get(className);
				break;
			}
			try {
				//Object proxy = (Object) BasicClassTypeUtil.createClassByUni(className);
				JceInputStream _is = new JceInputStream(data);
				_is.setServerEncoding(encodeName);
				return (Object) _is.read(proxy, 0, true);
			} catch (Exception ex) {
				ex.printStackTrace();
				throw new ObjectCreateException(ex);
			}
		}
	}

	/**
	 * 检测传入的元素类型
	 * 
	 * @param listTpye
	 * @param o
	 */
	private void checkObjectType(Vector listTpye, Object o) {
		if (o.getClass().isArray()) {
			byte[] bytes = (byte[]) o;
			if (bytes.length > 0) {
				listTpye.addElement("java.util.List");
				checkObjectType(listTpye, new Byte(bytes[0]));
			} else {
				listTpye.addElement("Array");
				listTpye.addElement("?");
			}
		} else if (o instanceof java.util.Vector) {
			listTpye.addElement("java.util.List");
			Vector list = (Vector) o;
			if (list.size() > 0) {
				checkObjectType(listTpye, list.elementAt(0));
			} else {
				listTpye.addElement("?");
				// throw new IllegalArgumentException("list  can not is empty");
			}
		} else if (o instanceof Hashtable) {
			listTpye.addElement("java.util.Map");
			Hashtable map = (Hashtable) o;
			if (map.size() > 0) {
				Enumeration keys = map.keys();
				while (keys.hasMoreElements()) {
					String key = (String) keys.nextElement();
					Object value = map.get(key);
					listTpye.addElement(key.getClass().getName());
					checkObjectType(listTpye, value);
				}
			} else {
				listTpye.addElement("?");
				listTpye.addElement("?");
				// throw new IllegalArgumentException("map  can not is empty");
			}
		} else if (o instanceof JceStruct) {
			listTpye.addElement(((JceStruct)o).className());
		} 
		else {
			listTpye.addElement(o.getClass().getName());
		}
	}

	public byte[] encode() throws IOException {
		JceOutputStream _os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		_os.write(_data, 0);
		return _os.getFlushBytes();
	}

	public void decode(byte[] buffer) throws IOException {
		//System.out.println("-------try to decode:"+WupHexUtil.bytes2HexStr(buffer));
		JceInputStream _is = new JceInputStream(buffer);
		_is.setServerEncoding(encodeName);
		Hashtable _tempdata = new Hashtable();
		Hashtable h = new Hashtable();
		h.put("", new byte[0]);
		_tempdata.put("", h);
		_data = (Hashtable) _is.readMap(_tempdata, 0, false);
	}

	// //////////////////////////////////////////////////////////////////////////////////////////

	public UniAttribute() {
	}

	public UniAttribute(java.util.Hashtable _data) {
		this._data = _data;
	}

	public boolean equals(Object o) {
		UniAttribute t = (UniAttribute) o;
		return (JceUtil.equals(1, t._data));
	}

	public void writeTo(JceOutputStream _os) throws IOException {
		_os.write(_data, 0);
	}

	public void readFrom(JceInputStream _is) throws IOException {
		_data.clear();
		String __var_1 = "";
		java.util.Hashtable __var_2 = new java.util.Hashtable();
		String __var_3 = "";
		byte[] __var_4 = (byte[]) new byte[1];
		byte __var_5 = 0;
		((byte[]) __var_4)[0] = __var_5;
		__var_2.put(__var_3, __var_4);
		_data.put(__var_1, __var_2);
		_data = (java.util.Hashtable) _is.read(_data, 0, true);

	}

	public void display(java.lang.StringBuffer _os, int _level) {
//		JceDisplayer _ds = new JceDisplayer(_os, _level);
//		_ds.display(_data, "_data");
	}

	public String className() {
		return "com.qq.jce.wup.UniAttribute";
	}

}
