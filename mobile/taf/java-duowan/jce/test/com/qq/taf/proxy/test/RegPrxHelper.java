package com.qq.taf.proxy.test;

import com.qq.taf.holder.JceBeanHolder;

public class RegPrxHelper implements RegPrx{

	public void getAllApplicationNames(JceBeanHolder<String> result) {
		System.out.println(result.getValue());
	}
	
	public static void main(String[] args) {
		RegPrxHelper h = new RegPrxHelper();
		h.getAllApplicationNames(new JceBeanHolder<String>("this is result"));
	}

}
