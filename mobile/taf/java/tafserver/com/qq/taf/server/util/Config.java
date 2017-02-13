package com.qq.taf.server.util;

import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * taf config
 * 使用限制：不能存在相同的path对应不同的value情况
 * 
 * @author easezhang
 *
 */
public class Config {
	private Node<String, String> root;
	
	private static final String TAG_STARTER = "<";
	
	private static final String TAG_OVER = "</";
	
	private static final String TAG_ENDER = "/>";
	
	private static final String TAG_CLOSE = ">";
	
	private static final String ATTR_FLAGF = "=";
	
	private static final String COMMENT = "#";
	
	private Map<String, String> values;
	
	private Map<String, String> escapedValues;
	

	private class Node<K, V> {
		Node<K, V> parent;
		V data;
		K key;
		HashMap<K, Node<K, V>> children = new HashMap<K, Node<K, V>>();
		
		public Node() {
		}
		
		public Node(K key, V data) {
			this.key = key;
			this.data = data;
		}
		
		public Node(K key, V data, Node<K, V> parent) {
			this(key, data);
			this.parent = parent;
		}
		
		
		public boolean hasChild(K key) {
			return children.containsKey(key);
		}
		
		public Node<K, V> addChild(K key, V value) {
			Node<K, V> node = new Node<K, V>(key, value, this);
			children.put(key, node);
			return node;
		}
		
		public Node<K, V> getChild(K key) {
			if(hasChild(key)) {
				return children.get(key);
			}
			else {
				return null;
			}
		}
		
		
		public String toString() {
			return data==null?"null":data.toString();
		}
		
	}
	
	private class Path {
		ArrayList<String> segs = new ArrayList<String>();
		int index = 0;
		
		public void add(String seg) {
			segs.add(seg);
			++index;
		}
		
		public void remove() {
			segs.remove(index-1);
			--index;
		}
		
		public String toString() {
			StringBuilder sb = new StringBuilder();
			for(int i=0;i<index;i++) {
				sb.append("/");
				sb.append(segs.get(i));
			}
			return sb.toString();
		}
	}
	
	public Config(String file, String charset) {
		try {
			root = new Node<String, String>();
			values = new HashMap<String, String>();
			escapedValues = new HashMap<String, String>();
			InputStreamReader reader=new InputStreamReader(new FileInputStream(file),charset);
			StringBuilder sb = new StringBuilder();
			char []chars = new char[1024];
			int len = 0; 
			while((len=reader.read(chars)) >= 0) {
				if(len > 0) {
					sb.append(new String(chars, 0, len));
				}
			}
			parse(sb.toString());
			//traverse();
		} catch (Throwable e) {
			e.printStackTrace();
			throw new RuntimeException("load config error");
		} 
	}
	
	public Config(String file) {
		this(file, "GBK");
	}

	
	private Node<String, String> findNode(String path) {
		String[] paths = path.split("/");
		Node<String, String> pointer = root;
		for(int i=0;i<paths.length;i++) {
			if(paths[i].length()==0) {
				continue;
			}
			if(pointer.hasChild(paths[i])) {
				pointer = pointer.getChild(paths[i]);
			}
			else {
				return null;
			}
		}
		return pointer;
	}
	
//	private String getKey(ArrayList<String> list) {
//		StringBuilder sb = new StringBuilder();
//		for(int i= list.size()-1;i>=0;i--) {
//			sb.append("/");
//			sb.append(list.get(i));
//		}
//		return sb.toString();
//	}
//
//	private void traverse() {
//		LinkedList<Node<String, String>> nodes = new LinkedList<Node<String, String>>();
//		nodes.add(root);
//		Node<String, String> tmpNode = null;
//		while((tmpNode = nodes.poll())!=null) {
//			if(tmpNode.isLeaf()) {
//				ArrayList<String> path = new ArrayList<String>();
//				Node<String, String> nowNode = tmpNode;
//				while(nowNode!=root) {
//					path.add(nowNode.key);
//					nowNode = nowNode.parent;
//				}
//				values.put(getKey(path), tmpNode.value());
//			}
//			nodes.addAll(tmpNode.children.values());
//		}
//	}
	
	public String get(String path, boolean escape) {
		String tmp = null;
		if(escape) {
			tmp = escapedValues.get(path);
		}
		else {
			tmp = values.get(path);
		}
		if(tmp==null) GeneralException.raise("config item ["+path+"] is absent" );
		return tmp;
	}
	
	public String get(String path) {
		String tmp = this.get(path, true);
		if(tmp==null) GeneralException.raise("config item ["+path+"] is absent" );
		return tmp;
	}
	
	public String get(String path, String defaultValue) {
		try {
			return get(path, true);
		}
		catch(Throwable th) {
			return defaultValue;
		}
	}
	
	public int getInt(String path) {
		String tmp = get(path);
		return Integer.valueOf(tmp);
	}
	
	public int getInt(String path, int defaultValue) {
		try {
			return getInt(path);
		}
		catch(Throwable th) {
			return defaultValue;
		}
	}
	
	public boolean getBool(String path) {
		String tmp = get(path);
		return Boolean.valueOf(tmp);
	}
	
	public boolean getBool(String path, boolean defaultValue) {
		try {
			return getBool(path);
		}
		catch(Throwable th) {
			return defaultValue;
		}
	}
	
	public List<String> getSubDomainNames(String path) {
		Node<String, String> node = this.findNode(path);
		List<String> retList = new ArrayList<String>();
		if(node != null) {
			Collection<Node<String, String>> collection = node.children.values();
			for(Node<String, String> n:collection) {
				if(n.key!=null && !n.key.startsWith(TAG_STARTER)) {
					retList.add(n.key);
				}
			}
			
		}
		return retList;
		
	}
	
	public Map<String,String> values() {
		return new HashMap<String, String>(escapedValues);
	}
	
	public Map<String,String> values(boolean escape) {
		if(escape) {
			return new HashMap<String, String>(escapedValues);
		}
		else {
			return new HashMap<String, String>(values);
		}
	}
	
	private void parse(String str) {
		String[] strs = str.split("\n");
		Node<String, String> pointer = root;
		Path path = new Path();
		for(int i=0;i<strs.length;i++) {
			pointer = this.parseLine(pointer, strs[i], path);
		}
		if(pointer != root) {
			throw new RuntimeException("tag <"+pointer.data+"> is not close");
		}
	}
	
	private String unEscape(String value) {
		return value.replaceAll("&amp;", "&").replaceAll("&nbsp;", " ").replaceAll("&lt;", "<").replaceAll("&gt;", ">").replaceAll("&quot;", "\"");
	}
	
	
	private Node<String, String> parseLine(Node<String, String> pointer, String str, Path path) {
		String tmp = str.trim();
		if(tmp.length() == 0||tmp.startsWith(COMMENT)) {
			return pointer;
		}
		if(tmp.startsWith(TAG_OVER) && tmp.endsWith(TAG_CLOSE)) {
			//"</abc>"
			String nodeName = tmp.substring(TAG_OVER.length(), tmp.length()-TAG_CLOSE.length());
			if(!pointer.data.equals(nodeName)) {
				throw new RuntimeException(nodeName +" is not match with start tag"+pointer.data);
			}
			path.remove();
			return pointer.parent;
		}
		else if(tmp.startsWith(TAG_STARTER) && tmp.endsWith(TAG_ENDER)) {
			//"<abc/>"
			String nodeName = tmp.substring(TAG_STARTER.length(), tmp.length()-TAG_ENDER.length());
			pointer.addChild(nodeName, nodeName);
			return pointer;
		}
		else if(tmp.startsWith(TAG_STARTER) && tmp.endsWith(TAG_CLOSE)) {
			//"<abc>"
			String nodeName = tmp.substring(TAG_STARTER.length(), tmp.length()-TAG_CLOSE.length());
			path.add(nodeName);
			return pointer.addChild(nodeName, nodeName);
		}
		else if(tmp.startsWith(TAG_STARTER) || tmp.endsWith(TAG_CLOSE)) {
			throw new RuntimeException("format error, "+tmp);
		}
		else {
			//a=b  a 
			int index = tmp.indexOf(ATTR_FLAGF);
			if(index == 0) {
				throw new RuntimeException("can not start with '=', " + tmp);
			}
			String key = "<";
			String value = "";
			if(index > 0) {
				key += tmp.substring(0, index).trim();
				value = tmp.substring(index+1).trim();
			}
			else {
				key += tmp.trim();
			}
			key+=">";
			pointer.addChild(key, value);
			values.put(path.toString()+key, value);
			escapedValues.put(path.toString()+key, this.unEscape(value));
			return pointer;
		}
		
	}
	
	public static void main(String...args) {
		Config config = new Config("e:/down/test.conf", "GBK");
		System.out.println(config.get("/taf/application/server<node>"));
		System.out.println(config.get("/taf/application/server<node>", false));
		//System.out.println(config.get("/taf/application/server<app>"));
		System.out.println("-------------------");
		//System.out.println(config.getSubDomainNames("/taf/application/server"));
		System.out.println("-------------------");
		Map<String, String> values = config.values();
		for(String key:values.keySet()) {
			System.out.println(key+":"+values.get(key));
		}
		values = config.escapedValues;
		for(String key:values.keySet()) {
			System.out.println(key+":"+values.get(key));
		}
		System.out.println(config.getInt("/taf/application/client<refresh-endpoint-interval>"));
		System.out.println(config.getBool("/taf/application/client<bool>"));
		System.out.println(config.get("/test<a>"));
	}
	
}


	


