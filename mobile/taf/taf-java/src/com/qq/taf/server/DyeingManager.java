package com.qq.taf.server;

/**
 * 染色管理器
 * @author fanzhang
 */
public class DyeingManager {
	
	private static final DyeingManager INSTANCE=new DyeingManager();
	private boolean isDyeing;
	private String dyeingKey;
	private String dyeingServant;
	private String dyeingInterface;
	
	private DyeingManager() {
	}
	
	public static DyeingManager getInstance() {
		return INSTANCE;
	}

	// notice: not synchronized 
	public void setDyeing(String dyeingKey,String dyeingServant,String dyeingInterface) {
		this.dyeingKey=dyeingKey;
		this.dyeingServant=dyeingServant;
		this.dyeingInterface=dyeingInterface;
		isDyeing=dyeingKey!=null && dyeingKey.trim().length()!=0;
	}

	// notice: not synchronized
	public boolean isDyeingReq(String key,String servant,String interfaze) {
		return dyeingKey.equals(key) && dyeingServant.equals(servant) && (dyeingInterface.equals("") || dyeingInterface.equals(interfaze));
	}
	
	// notice: not synchronized	
	public boolean isDyeing() {
		return isDyeing;
	}
	
}
