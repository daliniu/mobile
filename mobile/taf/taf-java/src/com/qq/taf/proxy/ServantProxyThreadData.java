package com.qq.taf.proxy;

import com.qq.taf.cnst.Const;

/**
 * 对客户端代理的线程私有数据的读写
 * @author fanzhang
 */
public class ServantProxyThreadData {
	
	public static class SampleKey {
		
	    public boolean _root;          
	    public String _unid;           
	    public int _depth;          
	    public int _width;          
	    public int _parentWidth;    
		    
		public SampleKey() {
			_root=true;
			_unid="";
			_depth=0;
			_width=0;
			_parentWidth=0;
		}
		
	}

	public static class ThreadPrivateData {
	    
	    public int _hashCode;  
	    public int _gridCode;  
	    public String _routeKey;  
	    public String _dyeingKey; 
	    public SampleKey _sampleKey; 

	    public ThreadPrivateData() {
	    	_hashCode=Const.INVALID_HASH_CODE;
	    	_gridCode=Const.INVALID_GRID_CODE;
	    	_routeKey="";
	    	_dyeingKey="";
	    	_sampleKey=new SampleKey();
	    }
	    
	}
	
	public static final ServantProxyThreadData INSTANCE=new ServantProxyThreadData();
	private ThreadLocal<ThreadPrivateData> threadPrivateData;
	
	private ServantProxyThreadData() {
		threadPrivateData=new ThreadLocal<ThreadPrivateData>() {
			@Override
			protected ThreadPrivateData initialValue() {
				return new ThreadPrivateData();
			}
		};
	}

	public void setData(ThreadPrivateData data) {
		threadPrivateData.set(data);
	}
	
	public ThreadPrivateData getData() {
		return threadPrivateData.get();
	}
	
}