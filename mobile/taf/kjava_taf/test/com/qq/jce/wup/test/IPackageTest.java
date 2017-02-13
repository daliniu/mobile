package com.qq.jce.wup.test;

import java.io.IOException;

import MiniGameProto.MSGTYPE;
import MiniGameProto.TMsgHead;
import MiniGameProto.TPackage;

import com.qq.taf.jce.JceInputStream;
import com.qq.taf.jce.JceOutputStream;

public class IPackageTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			TPackage nt = new TPackage();
			nt.iVersion = 100; // 通讯版本号
			nt.iUin = 10000; // qq号
			nt.iFlag = 0; // iFlag?
			TMsgHead mh = new TMsgHead();
			mh.nMsgID = 10001;
			mh.nMsgType = MSGTYPE._MSGTYPE_REQUEST;
			mh.nMsgLen = 10;
			nt.vecMsgHead.addElement(mh);
			nt.vecMsgData = new byte[]{1,2,3,4,5,6,7};
			JceOutputStream _out2 = new JceOutputStream();
			nt.writeTo(_out2);
			byte[] _sBuffer2 = _out2.getFlushBytes();
			System.out.println(HexUtils.convert(_sBuffer2));
			TPackage proxy = new TPackage();
			JceInputStream _is = new JceInputStream(_sBuffer2);
			proxy.readFrom(_is);
			System.out.println(proxy.vecMsgHead.size());
			if ( proxy.vecMsgHead.size() > 0) {
				TMsgHead head = (TMsgHead)proxy.vecMsgHead.elementAt(0);
				System.out.println(head.nMsgID);
				System.out.println(head.nMsgLen);
				System.out.println(head.nMsgType);
			}
			System.out.println(HexUtils.convert(proxy.vecMsgData));

//			JceOutputStream _out = new JceOutputStream();
//			_out.write(nt, 0);
//			byte[] _sBuffer = _out.getFlushBytes();
//			System.out.println(HexUtils.convert(_sBuffer));
			
			
//			String digits = "0035122236B3C720013C4900010A00031002200B0B5D00000B0C16083330313631363531";
//			byte[] bytes = HexUtils.convert(digits);
//			TPackage proxy = new TPackage();
//			
//			JceInputStream _is = new JceInputStream(bytes);
//			proxy.readFrom(_is);
//			System.out.println(proxy.vecMsgHead.size());
//			if ( proxy.vecMsgHead.size() > 0) {
//				TMsgHead head = (TMsgHead)proxy.vecMsgHead.elementAt(0);
//				System.out.println(head.nMsgID);
//				System.out.println(head.nMsgLen);
//				System.out.println(head.nMsgType);
//			}
//			System.out.println(proxy.getVecMsgData().length);
			//TPackage ttt = (TPackage) _is.read(proxy, 0, true);
//			
//			UniPacket client = new UniPacket();
//			client.setRequestId(1);
//			client.setServantName("1");
//			client.setFuncName("2");
//			TPackage t = new TPackage();
//			t.iVersion = 100; // 通讯版本号
//			t.iUin = 10000; // qq号
//			t.iFlag = 0; // iFlag?
//			TMsgHead mh = new TMsgHead();
//			mh.nMsgID = 10001;
//			mh.nMsgType = MSGTYPE._MSGTYPE_REQUEST;
//			mh.nMsgLen = 10;
//			t.vecMsgHead = new TMsgHead[1];
//			t.vecMsgHead[0] = mh;
//			t.vecMsgData = new byte[0];
//			client.put("TPackage", t);
//			byte[] vEncodedData = client.encode();
//			System.out.println(HexUtils.convert(vEncodedData));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
