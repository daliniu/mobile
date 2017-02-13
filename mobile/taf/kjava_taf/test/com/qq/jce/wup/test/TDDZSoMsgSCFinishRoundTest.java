package com.qq.jce.wup.test;

import java.io.IOException;
import java.util.Vector;

import MiniGameProto.TPackage;

import com.qq.ddz.MiniGameProtoDDZ.TDDZSoMsgSCFinishRound;
import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

public class TDDZSoMsgSCFinishRoundTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			TDDZSoMsgSCFinishRound nt = new TDDZSoMsgSCFinishRound();
			nt.ivecUserScore = new int[]{1};
			nt.vecRestCards = new Vector();
			nt.vecRestCards.addElement(new byte[]{2});
			JceOutputStream _out2 = new JceOutputStream();
			nt.writeTo(_out2);
			byte[] _sBuffer2 = _out2.getFlushBytes();
			System.out.println(HexUtils.convert(_sBuffer2));
			TDDZSoMsgSCFinishRound proxy = new TDDZSoMsgSCFinishRound();
			JceInputStream _is = new JceInputStream(_sBuffer2);
			proxy.readFrom(_is);
			System.out.println(proxy.getVecRestCards());
			if ( proxy.getVecRestCards().size() > 0 ) {
				System.out.println(proxy.getVecRestCards().elementAt(0));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

}
