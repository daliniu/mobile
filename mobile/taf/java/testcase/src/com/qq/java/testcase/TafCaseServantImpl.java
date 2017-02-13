package com.qq.java.testcase;

import static com.qq.taf.server.ApplicationUtil.*;

import com.qq.java.testcase.testcaseservant.UserInfo;
import com.qq.java.testcase.testcaseservant.UserInfoHolder;
import com.qq.taf.holder.JceIntHolder;
import com.qq.taf.holder.JceLongHolder;
import com.qq.taf.holder.JceShortHolder;
import com.qq.taf.server.JceCurrent;

public class TafCaseServantImpl extends com.qq.java.testcase.testcaseservant.TafCaseServant {
	
	public int test(JceCurrent jc) {
		return 0;
	}
	
	@Override
	public int getUser(UserInfo inUser, UserInfoHolder outUser, JceCurrent _jc) {
		System.out.println("=== in user ===");
		System.out.println(inUser.toString());
		UserInfo ui=new UserInfo();
		ui.ubQQ=inUser.ubQQ;
		ui.usQQ=inUser.usQQ;
		ui.uiQQ=inUser.uiQQ;
		ui.uiTest=inUser.uiTest;
		outUser.value=ui;
		return 0;
	}

	@Override
	public int testUnsigned(short ubInUin, int usInUin, long uiInUin,JceShortHolder ubOutUin, JceIntHolder usOutUin,JceLongHolder uiOutUin, JceCurrent _jc) {
		System.out.println("=== in vars ===");
		System.out.println(ubInUin+" "+usInUin+" "+uiInUin);
		ubOutUin.value=ubInUin;
		usOutUin.value=usInUin;
		uiOutUin.value=uiInUin;
		return 0;
	}
	
}
