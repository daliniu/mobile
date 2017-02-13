package com.duowan.taf.jce;

import java.util.Collection;
import java.util.List;
import java.util.Map;

/**
 * 格式化输出jce结构的所有属性
 * 主要用于调试或打日志
 * @author meteorchen
 *
 */
public final class JceDisplayer
{
	private StringBuilder	sb;
	private int				_level = 0;
	
	private void ps(String fieldName)
	{
		for(int i = 0; i < _level; ++i)
			sb.append('\t');
		if(fieldName != null)
			sb.append(fieldName).append(": ");
	}
	
	public JceDisplayer(StringBuilder sb, int level)
	{
		this.sb = sb;
		this._level = level;
	}
	
	public JceDisplayer(StringBuilder sb)
	{
		this.sb = sb;
	}

	public JceDisplayer display(boolean b, String fieldName)
	{
		ps(fieldName);
		sb.append(b ? 'T' : 'F').append('\n');
		return this;
	}

	public JceDisplayer display(byte n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}
	
	public JceDisplayer display(char n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(short n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(int n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(long n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(float n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(double n, String fieldName)
	{
		ps(fieldName);
		sb.append(n).append('\n');
		return this;
	}

	public JceDisplayer display(String s, String fieldName)
	{
		ps(fieldName);
		if (null == s ) {
			sb.append("null").append('\n');
		} else {
			sb.append(s).append('\n');
		}
		
		return this;
	}
	
	public JceDisplayer display(byte[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(byte o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(char[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(char o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(short[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(short o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(int[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(int o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(long[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(long o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(float[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(float o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public JceDisplayer display(double[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(double o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}
	
	public <K, V> JceDisplayer display(Map<K, V> m, String fieldName)
	{
		ps(fieldName);
		if (null == m ) {
			sb.append("null").append('\n');
			return this;
		}
		if(m.isEmpty()){
			sb.append(m.size()).append(", {}").append('\n');
			return this;
		}
		sb.append(m.size()).append(", {").append('\n');
		JceDisplayer jd1 = new JceDisplayer(sb, _level + 1);
		JceDisplayer jd = new JceDisplayer(sb, _level + 2);
		for(Map.Entry<K, V> en : m.entrySet()){
			jd1.display('(', null);
			jd.display(en.getKey(), null);
			jd.display(en.getValue(), null);
			jd1.display(')', null);
		}
		display('}', null);
		return this;
	}
	
	public <T> JceDisplayer display(T[] v, String fieldName)
	{
		ps(fieldName);
		if (null == v ) {
			sb.append("null").append('\n');
			return this;
		}
		if(v.length == 0){
			sb.append(v.length).append(", []").append('\n');
			return this;
		}
		sb.append(v.length).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(T o : v)
			jd.display(o, null);
		display(']', null);
		return this;
	}

	public <T> JceDisplayer display(Collection<T> v, String fieldName)
	{
		if (null == v ) {
			ps(fieldName);
			sb.append("null").append('\t');
			return this;
		} else {
			return display(v.toArray(), fieldName);
		}
		
		/*
		ps(fieldName);
		if(v.isEmpty()){
			sb.append(v.size()).append(", []").append('\n');
			return this;
		}
		sb.append(v.size()).append(", [").append('\n');
		JceDisplayer jd = new JceDisplayer(sb, _level + 1);
		for(T o : v)
			jd.display(o, null);
		display(']', null);
		return this;
		*/
	}
	
	@SuppressWarnings("unchecked")
	public <T> JceDisplayer display(T o, String fieldName)
	{
		if (null == o ) {
			sb.append("null").append('\n');
		} else if(o instanceof Byte){
			display(((Byte) o).byteValue(), fieldName);
		}else if(o instanceof Boolean){
			display(((Boolean) o).booleanValue(), fieldName);
		}else if(o instanceof Short){
			display(((Short) o).shortValue(), fieldName);
		}else if(o instanceof Integer){
			display(((Integer) o).intValue(), fieldName);
		}else if(o instanceof Long){
			display(((Long) o).longValue(), fieldName);
		}else if(o instanceof Float){
			display(((Float) o).floatValue(), fieldName);
		}else if(o instanceof Double){
			display(((Double) o).doubleValue(), fieldName);
		}else if(o instanceof String){
			display((String) o, fieldName);
		}else if(o instanceof Map){
			display((Map) o, fieldName);
		}else if(o instanceof List){
			display((List) o, fieldName);
		}else if(o instanceof JceStruct){
			display((JceStruct) o, fieldName);
		} else if (o instanceof byte[]) {
			display((byte[])o, fieldName);
		} else if (o instanceof boolean[]) {
			display((boolean[])o, fieldName);
		} else if (o instanceof short[]) {
			display((short[])o, fieldName);
		} else if (o instanceof int[]) {
			display((int[])o, fieldName);
		} else if (o instanceof long[]) {
			display((long[])o, fieldName);
		} else if (o instanceof float[]) {
			display((float[])o, fieldName);
		} else if (o instanceof double[]) {
			display((double[])o, fieldName);
		} else if (o.getClass().isArray()) {
			display((Object[])o, fieldName);
		}else{
			throw new JceEncodeException("write object error: unsupport type.");
		}
		return this;
	}

	public JceDisplayer display(JceStruct v, String fieldName)
	{
		display('{', fieldName);
		if (null == v ) {
			sb.append('\t').append("null");
		} else {
			v.display(sb, _level + 1);
		}
		
		display('}', null);
		return this;
	}

	public static void main(String[] args)
	{
		StringBuilder sb = new StringBuilder();
		sb.append(1.2);
		System.out.println(sb.toString());
	}
}
