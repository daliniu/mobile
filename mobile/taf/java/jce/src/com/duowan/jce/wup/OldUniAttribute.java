package com.duowan.jce.wup;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.duowan.taf.jce.JceInputStream;
import com.duowan.taf.jce.JceOutputStream;
import com.duowan.taf.jce.JceStruct;
import com.duowan.taf.jce.JceUtil;

class OldUniAttribute  {
		
	protected HashMap<String, HashMap<String, byte[]>> _data = new HashMap<String, HashMap<String, byte[]>>();
	
	/**
	 * 存储get的时候传入的对象类型 避免每次都重新生成
	 */
	protected HashMap<String,Object> cachedClassName = new HashMap<String,Object>();
	/**
	 * 存储get后的数据 避免多次解析
	 */
	private HashMap<String,Object> cachedData = new HashMap<String,Object>();
	
	
	protected String encodeName = "UTF-8";
	
	JceInputStream _is = new JceInputStream();
	
	/**
	 * 返回编码字符集 默认为UTF8
	 * @return
	 */
	public String getEncodeName() {
		return encodeName;
	}

	/**
	 * 设置编码字符集 默认为UTF8
	 * @param encodeName
	 */
	public void setEncodeName(String encodeName) {
		this.encodeName = encodeName;
	}
	
	/**
	 * 清除缓存的解析过的数据
	 */
	public void clearCacheData() {
		cachedData.clear();
	}
	
	/**
	 * 返回放入数据结构中的所有key
	 * @return
	 */
	public Set<String> getKeySet() {
		return Collections.unmodifiableSet(_data.keySet());
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
	 * @param <T>
	 * @param name
	 * @param t
	 */
	public <T> void put(String name, T t) {
		if (name == null) {
			throw new IllegalArgumentException("put key can not is null");
		}
		if (t == null) {
			throw new IllegalArgumentException("put value can not is null");
		}
		if (t instanceof Set) {
			throw new IllegalArgumentException("can not support Set");
		}
		JceOutputStream _out = new JceOutputStream();
		_out.setServerEncoding(encodeName);
		_out.write(t, 0);
		byte[] _sBuffer = JceUtil.getJceBufArray(_out.getByteBuffer());
		HashMap<String, byte[]> pair = new HashMap<String, byte[]>(1);
		ArrayList<String> listTpye = new ArrayList<String>(1);
		checkObjectType(listTpye, t);
		String className = BasicClassTypeUtil.transTypeList(listTpye);
		pair.put(className, _sBuffer);
		cachedData.remove(name);
		_data.put(name, pair);
	}


	/**
	 * 获取一个元素
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T getJceStruct(String name) throws ObjectCreateException {
		if (!_data.containsKey(name)) {
			return null;
		} else if (cachedData.containsKey(name)) {
			return (T) cachedData.get(name);
		} else {
			HashMap<String, byte[]> pair = _data.get(name);
			String className = null;
			byte[] data = new byte[0];
			for (Entry<String, byte[]> e : pair.entrySet()) {
				className = e.getKey();
				data = e.getValue();
				break;
			}
			try {
				T proxy = (T) getCacheProxy(className);
				_is.warp(data);
				_is.setServerEncoding(encodeName);
				//System.out.println("try to read proxy "+proxy.getClass().getName()+" name is "+className+" data:"+WupHexUtil.bytes2HexStr(data));
				JceStruct o =  _is.directRead((JceStruct)proxy, 0, true);
				saveDataCache(name,o);
				return (T) o ;
			} catch (Exception ex) {
				ex.printStackTrace();
				throw new ObjectCreateException(ex);
			}
		}
	}
	
	
	/**
	 * 获取一个元素
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T get(String name) throws ObjectCreateException {
		if (!_data.containsKey(name)) {
			return null;
		} else if (cachedData.containsKey(name)) {
			return (T) cachedData.get(name);
		} else {
			HashMap<String, byte[]> pair = _data.get(name);
			String className = null;
			byte[] data = new byte[0];
			for (Entry<String, byte[]> e : pair.entrySet()) {
				className = e.getKey();
				data = e.getValue();
				break;
			}
			try {
				T proxy = (T) getCacheProxy(className);
				_is.warp(data);
				_is.setServerEncoding(encodeName);
				//System.out.println("try to read proxy "+proxy.getClass().getName()+" name is "+className+" data:"+WupHexUtil.bytes2HexStr(data));
				Object o =  _is.read(proxy, 0, true);
				saveDataCache(name,o);
				return (T) o ;
			} catch (Exception ex) {
				ex.printStackTrace();
				throw new ObjectCreateException(ex);
			}
		}
	}

	private Object getCacheProxy(String className) {
		Object proxy  = null ;
		if ( cachedClassName.containsKey(className) ) {
			proxy = cachedClassName.get(className);
		} else {
			proxy =  BasicClassTypeUtil.createClassByUni(className);
			cachedClassName.put(className, proxy);
		}
		return proxy;
	}
	
	private void saveDataCache(String name , Object o ) {
		cachedData.put(name, o);
	}
	
	/**
	 * 获取一个元素
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T get(String name, Object defaultValue)  {
		if (!_data.containsKey(name)) {
			return (T) defaultValue;
		} else if (cachedData.containsKey(name)) {
			return (T) cachedData.get(name);
		}  else {
			HashMap<String, byte[]> pair = _data.get(name);
			String className = "";
			byte[] data = new byte[0];
			for (Entry<String, byte[]> e : pair.entrySet()) {
				className = e.getKey();
				data = e.getValue();
				break;
			}
			try {
				T proxy = (T) getCacheProxy(className);
				_is.warp(data);
				_is.setServerEncoding(encodeName);
				//System.out.println("try to read proxy "+proxy.getClass().getName()+" name is "+className+" data:"+WupHexUtil.bytes2HexStr(data));
				Object o =  _is.read(proxy, 0, true);
				saveDataCache(name,o);
				return (T) o;
			} catch (Exception ex) {
				ex.printStackTrace();
				saveDataCache(name,defaultValue);
				return (T)defaultValue;
				//throw new ObjectCreateException(ex);
			}
		}
	}

	/**
	 * 删除一个元素
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T remove(String name) throws ObjectCreateException {
		if (!_data.containsKey(name)) {
			return null;
		} else {
			HashMap<String, byte[]> pair = _data.remove(name);
			String className = "";
			byte[] data = new byte[0];
			for (Entry<String, byte[]> e : pair.entrySet()) {
				className = e.getKey();
				data = e.getValue();
				break;
			}
			try {
				T proxy = (T) BasicClassTypeUtil.createClassByUni(className);
				_is.warp(data);
				_is.setServerEncoding(encodeName);
				return (T) _is.read(proxy, 0, true);
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
	@SuppressWarnings("unchecked")
	private void checkObjectType(ArrayList<String> listTpye, Object o) {
		if (o.getClass().isArray()) {
			if ( !o.getClass().getComponentType().toString().equals("byte")) {
				throw new IllegalArgumentException("only byte[] is supported");
			}
			if ( Array.getLength(o)  > 0 ) { //byte[]里面有元素 编译成list<char>
				listTpye.add("java.util.List");
				checkObjectType(listTpye, Array.get(o,0));
			} else {
				listTpye.add("Array");
				listTpye.add("?");
				//throw new IllegalArgumentException("array  can not is empty");
			}
		} else if (o instanceof Array) {
			throw new java.lang.IllegalArgumentException("can not support Array, please use List");
		} else if (o instanceof List) {
			listTpye.add("java.util.List");
			List list = (List) o;
			if (list.size() > 0) {
				checkObjectType(listTpye, list.get(0));
			} else {
				listTpye.add("?");
				//throw new IllegalArgumentException("list  can not is empty");
			}
		} else if (o instanceof Map) {
			listTpye.add("java.util.Map");
			Map map = (Map) o;
			if (map.size() > 0) {
				Iterator it = map.keySet().iterator();
				Object key = it.next();
				Object value = map.get(key);
				listTpye.add(key.getClass().getName());
				checkObjectType(listTpye, value);
			} else {
				listTpye.add("?");
				listTpye.add("?");
				//throw new IllegalArgumentException("map  can not is empty");
			}
		} else {
			listTpye.add(o.getClass().getName());
		}
	}
	
	public byte[] encode() {
		JceOutputStream _os = new JceOutputStream(0);
		_os.setServerEncoding(encodeName);
		_os.write(_data, 0);
		return JceUtil.getJceBufArray(_os.getByteBuffer());
	}

	public void decode(byte[] buffer) {
		_is.warp(buffer);
		_is.setServerEncoding(encodeName);
		HashMap<String, HashMap<String, byte[]>> _tempdata = new HashMap<String, HashMap<String, byte[]>>(1);
		HashMap<String, byte[]> h = new HashMap<String, byte[]>(1);
		h.put("", new byte[0]);
		_tempdata.put("", h);
		_data = (HashMap<String, HashMap<String, byte[]>>) _is.readMap(_tempdata, 0, false);
	}
 
	
}
