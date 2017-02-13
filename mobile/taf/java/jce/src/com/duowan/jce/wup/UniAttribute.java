package com.duowan.jce.wup;

import java.util.Collections;
import java.util.HashMap;
import java.util.Set;
import java.util.Map.Entry;

import com.duowan.taf.jce.JceInputStream;
import com.duowan.taf.jce.JceOutputStream;
import com.duowan.taf.jce.JceUtil;


public class UniAttribute extends OldUniAttribute {
		
	protected HashMap<String, byte[]> _newData = null;

	/**
	 * 存储get后的数据 避免多次解析
	 */
	private HashMap<String,Object> cachedData = new HashMap<String,Object>();
	
	
	JceInputStream _is = new JceInputStream();
	
	/**
	 * 使用老版本编码
	 */
	public void useVersion3() {
		_newData = new HashMap<String, byte[]>();
		
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
		if (null != _newData) {
			return Collections.unmodifiableSet(_newData.keySet());
		}
		return Collections.unmodifiableSet(_data.keySet());
	}
	
	public boolean isEmpty() {
		if (null != _newData) {
			return _newData.isEmpty();
		}
		return _data.isEmpty();
	}
	
	public int size() {
		if (null != _newData) {
			return _newData.size();
		}
		return _data.size();
	}
	
	public boolean containsKey(String key) {
		if (null != _newData) {
			return _newData.containsKey(key);
		}
		return _data.containsKey(key);
	}
	
	/**
	 * 放入一个元素
	 * @param <T>
	 * @param name
	 * @param t
	 */
	public <T> void put(String name, T t) {
		if (null != _newData ) {
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
			_newData.put(name, _sBuffer);
		} else {
			super.put(name, t);
		}
	}
	
	/**
	 * 获取一个元素
	 * 版本3中新增
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T getJceStruct(String name,T proxy) throws ObjectCreateException {
		if (!_newData.containsKey(name)) {
			return null;
		} else if (cachedData.containsKey(name)) {
			return (T) cachedData.get(name);
		} else {
			byte[] data = _newData.get(name);
			try {
				Object o = decodeData(data,proxy);
				if (null != o ) {
					saveDataCache(name,o);
				}
				return (T) o ;
			} catch (Exception ex) {
				throw new ObjectCreateException(ex);
			}
		}
	}
	
	
	/**
	 * 自行创建Proxy 避免反射
	 * 版本3中新增
	 * @param <T>
	 * @param name
	 * @param proxy
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T getByClass(String name, T proxy) throws ObjectCreateException {
		if ( null != _newData) {
			if (!_newData.containsKey(name)) {
				return null;
			} else if (cachedData.containsKey(name)) {
				return (T) cachedData.get(name);
			} else {
				byte[] data = _newData.get(name);
				try {
					Object o = decodeData(data,proxy);
					if (null != o ) {
						saveDataCache(name,o);
					}
					return (T) o ;
				} catch (Exception ex) {
					throw new ObjectCreateException(ex);
				}
			}
		} else {
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
					_is.warp(data);
					_is.setServerEncoding(encodeName);
					Object o =  _is.read(proxy, 0, true);
					saveDataCache(name,o);
					return (T) o ;
				} catch (Exception ex) {
					throw new ObjectCreateException(ex);
				}
			}
		}
		
	}
	
	/**
	 * 自行创建Proxy 避免反射 支持默认值
	 * 版本3中新增
	 * @param <T>
	 * @param name
	 * @param proxy
	 * @param defaultValue
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T getByClass(String name, T proxy,T defaultValue) throws ObjectCreateException {
		if ( null != _newData) {
			if (!_newData.containsKey(name)) {
				return defaultValue;
			} else if (cachedData.containsKey(name)) {
				return (T) cachedData.get(name);
			} else {
				byte[] data = _newData.get(name);
				try {
					Object o = decodeData(data,proxy);
					if (null != o ) {
						saveDataCache(name,o);
					}
					return (T) o ;
				} catch (Exception ex) {
					throw new ObjectCreateException(ex);
				}
			}
		} else {
			if (!_data.containsKey(name)) {
				return defaultValue;
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
					_is.warp(data);
					_is.setServerEncoding(encodeName);
					Object o =  _is.read(proxy, 0, true);
					saveDataCache(name,o);
					return (T) o ;
				} catch (Exception ex) {
					throw new ObjectCreateException(ex);
				}
			}
		}
		
	}
	
	/**
	 * 获取一个元素 
	 * 如不存在 返回默认值
	 * 版本3中新增
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T get(String name, T proxy, Object defaultValue)  {
		if (!_newData.containsKey(name)) {
			return (T) defaultValue;
		} 
		return getByClass(name,proxy);
	}
	
	
	private Object decodeData(byte[] data,Object proxy) {
		_is.warp(data);
		_is.setServerEncoding(encodeName);
		Object o =  _is.read(proxy, 0, true);
		return o ;
	}
	
	
	private void saveDataCache(String name , Object o ) {
		cachedData.put(name, o);
	}
	
	/**
	 * 获取一个元素
	 * 如果收到的数据是版本3的编码 会抛异常
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T getJceStruct(String name) throws ObjectCreateException {
		if (null != _newData) {
			throw new RuntimeException("data is encoded by new version, please use getJceStruct(String name,T proxy)");
		} else {
			return (T)super.getJceStruct(name);
		}
	}
	
	/**
	 * 获取一个元素
	 * 如果收到的数据是版本3的编码 会抛异常
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T get(String name) throws ObjectCreateException {
		if (null != _newData) {
			throw new RuntimeException("data is encoded by new version, please use getByClass(String name, T proxy)");
		} else {
			return (T)super.get(name);
		}
	}
	
	/**
	 * 获取一个元素
	 * 如果收到的数据是版本3的编码 会抛异常
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T get(String name, Object defaultValue)  {
		if (null != _newData) {
			throw new RuntimeException("data is encoded by new version, please use get(String name, T proxy, Object defaultValue)");
		} else {
			return (T)super.get(name,defaultValue);
		}
	}
	
	/**
	 * 删除一个元素
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	public <T> T remove(String name) throws ObjectCreateException {
		if (null != _newData ) {
			//用版本3编码 直接从_newData中删除
			if (!_newData.containsKey(name)) {
				return null;
			} else {
				_newData.remove(name);
				return null ;
			}
		} else {
			return (T)super.remove(name);
		}
		
	}
	
	/**
	 * 删除一个元素
	 * 版本3新增
	 * @param <T>
	 * @param name
	 * @return
	 * @throws ObjectCreateException
	 */
	@SuppressWarnings("unchecked")
	public <T> T remove(String name,T proxy) throws ObjectCreateException {
		//用版本3编码 直接从_newData中删除
		if (!_newData.containsKey(name)) {
			return null;
		} else {
			if (null != proxy) {
				byte[] data = _newData.remove(name);
				Object o = decodeData(data,proxy);
				return (T) o ;
			} else {
				_newData.remove(name);
				return null ;
			}
		}
	}
	
	public byte[] encode() {
		if (null != _newData ) {
			JceOutputStream _os = new JceOutputStream(0);
			_os.setServerEncoding(encodeName);
			_os.write(_newData, 0);
			return JceUtil.getJceBufArray(_os.getByteBuffer());
		} 
		return super.encode();
	}
	
	/**
	 * 明确知道是version3的编码
	 * @param buffer
	 */
	public void decodeVersion3(byte[] buffer) {
		_is.warp(buffer);
		_is.setServerEncoding(encodeName);
		HashMap<String, byte[]> _tempdata = new HashMap<String, byte[]>(1);
		_tempdata.put("",  new byte[0]);
		_newData = (HashMap<String, byte[]>) _is.readMap(_tempdata, 0, false);
	}
	
	/**
	 * 明确知道是Version2的版本
	 * @param buffer
	 */
	public void decodeVersion2(byte[] buffer) {
		super.decode(buffer);
	}

	/**
	 * 解码的时候不知道版本
	 * 先照着旧的解 出错再用新的
	 */
	public void decode(byte[] buffer) {
		try {
			super.decode(buffer);
			return ;
		} catch (Exception e) {
			_is.warp(buffer);
			_is.setServerEncoding(encodeName);
			HashMap<String, byte[]> _tempdata = new HashMap<String, byte[]>(1);
			_tempdata.put("",  new byte[0]);
			_newData = (HashMap<String, byte[]>) _is.readMap(_tempdata, 0, false);
		}
	}

	
}
