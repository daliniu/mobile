package MTT;

import java.io.IOException;
import java.util.Enumeration;

import com.qq.jce.wup.UniPacket;
import com.qq.jce.wup.test.HexUtils;
import com.qq.taf.jce.JceInputStream;

public class LoginRespTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			byte[] rightData = {
					(byte)0, (byte)0, (byte)0, (byte)-35, (byte)16, (byte)2, (byte)44, (byte)60, (byte)64, (byte)2, (byte)86, (byte)5, (byte)108, (byte)111, (byte)103, (byte)105, (byte)110, (byte)102, (byte)5, (byte)108, (byte)111, (byte)103, (byte)105, (byte)110, (byte)125, (byte)0, (byte)1, (byte)0, (byte)-69, (byte)8, (byte)0, (byte)2, (byte)6, (byte)0, (byte)24, (byte)0, (byte)1, (byte)6, (byte)5, (byte)105, (byte)110, (byte)116, (byte)51, (byte)50, (byte)29, (byte)0, (byte)0, (byte)1, (byte)12, (byte)6, (byte)6, (byte)115, (byte)116, (byte)76, (byte)82, (byte)115, (byte)112, (byte)24, (byte)0, (byte)1, (byte)6, (byte)12, (byte)77, (byte)84, (byte)84, (byte)46, (byte)76, (byte)111, (byte)103, (byte)105, (byte)110, (byte)82, (byte)115, (byte)112, (byte)29, (byte)0, (byte)1, (byte)0, (byte)-119, (byte)10, (byte)13, (byte)0, (byte)0, (byte)16, (byte)90, (byte)-67, (byte)43, (byte)39, (byte)-66, (byte)51, (byte)79, (byte)-89, (byte)-95, (byte)27, (byte)114, (byte)-68, (byte)64, (byte)-13, (byte)-63, (byte)-93, (byte)29, (byte)0, (byte)0, (byte)48, (byte)53, (byte)101, (byte)56, (byte)57, (byte)98, (byte)101, (byte)49, (byte)54, (byte)101, (byte)98, (byte)54, (byte)50, (byte)98, (byte)55, (byte)50, (byte)48, (byte)48, (byte)53, (byte)99, (byte)98, (byte)54, (byte)53, (byte)97, (byte)52, (byte)100, (byte)52, (byte)55, (byte)49, (byte)100, (byte)97, (byte)57, (byte)101, (byte)54, (byte)55, (byte)55, (byte)98, (byte)97, (byte)57, (byte)52, (byte)54, (byte)57, (byte)49, (byte)51, (byte)52, (byte)54, (byte)100, (byte)57, (byte)56, (byte)34, (byte)75, (byte)-14, (byte)62, (byte)-96, (byte)57, (byte)0, (byte)2, (byte)6, (byte)20, (byte)49, (byte)49, (byte)55, (byte)46, (byte)49, (byte)51, (byte)53, (byte)46, (byte)49, (byte)50, (byte)56, (byte)46, (byte)49, (byte)54, (byte)58, (byte)50, (byte)56, (byte)48, (byte)48, (byte)48, (byte)6, (byte)20, (byte)49, (byte)49, (byte)55, (byte)46, (byte)49, (byte)51, (byte)53, (byte)46, (byte)49, (byte)50, (byte)56, (byte)46, (byte)50, (byte)48, (byte)58, (byte)50, (byte)56, (byte)48, (byte)48, (byte)48, (byte)73, (byte)12, (byte)90, (byte)12, (byte)22, (byte)0, (byte)38, (byte)0, (byte)54, (byte)0, (byte)11, (byte)11, (byte)-116, (byte)-104, (byte)12, (byte)-88, (byte)12
				};
			byte[] errorData = {
					(byte)0, (byte)0, (byte)0, (byte)-28, (byte)16, (byte)2, (byte)44, (byte)60, (byte)64, (byte)2, (byte)86, (byte)5, (byte)108, (byte)111, (byte)103, (byte)105, (byte)110, (byte)102, (byte)5, (byte)108, (byte)111, (byte)103, (byte)105, (byte)110, (byte)125, (byte)0, (byte)1, (byte)0, (byte)-62, (byte)8, (byte)0, (byte)2, (byte)6, (byte)0, (byte)24, (byte)0, (byte)1, (byte)6, (byte)5, (byte)105, (byte)110, (byte)116, (byte)51, (byte)50, (byte)29, (byte)0, (byte)0, (byte)1, (byte)12, (byte)6, (byte)6, (byte)115, (byte)116, (byte)76, (byte)82, (byte)115, (byte)112, (byte)24, (byte)0, (byte)1, (byte)6, (byte)12, (byte)77, (byte)84, (byte)84, (byte)46, (byte)76, (byte)111, (byte)103, (byte)105, (byte)110, (byte)82, (byte)115, (byte)112, (byte)29, (byte)0, (byte)1, (byte)0, (byte)-112, (byte)10, (byte)13, (byte)0, (byte)0, (byte)16, (byte)117, (byte)-57, (byte)-37, (byte)-12, (byte)-116, (byte)-13, (byte)64, (byte)112, (byte)-80, (byte)-34, (byte)-68, (byte)-84, (byte)-110, (byte)4, (byte)-58, (byte)84, (byte)29, (byte)0, (byte)0, (byte)48, (byte)51, (byte)51, (byte)50, (byte)102, (byte)55, (byte)51, (byte)100, (byte)56, (byte)53, (byte)55, (byte)101, (byte)49, (byte)100, (byte)49, (byte)52, (byte)53, (byte)48, (byte)53, (byte)99, (byte)98, (byte)54, (byte)53, (byte)97, (byte)52, (byte)100, (byte)52, (byte)55, (byte)49, (byte)100, (byte)97, (byte)57, (byte)101, (byte)54, (byte)55, (byte)55, (byte)98, (byte)97, (byte)57, (byte)52, (byte)54, (byte)57, (byte)49, (byte)51, (byte)52, (byte)54, (byte)100, (byte)57, (byte)56, (byte)34, (byte)75, (byte)-14, (byte)68, (byte)-101, (byte)57, (byte)0, (byte)2, (byte)6, (byte)20, (byte)49, (byte)49, (byte)55, (byte)46, (byte)49, (byte)51, (byte)53, (byte)46, (byte)49, (byte)50, (byte)56, (byte)46, (byte)49, (byte)54, (byte)58, (byte)50, (byte)56, (byte)48, (byte)48, (byte)48, (byte)6, (byte)20, (byte)49, (byte)49, (byte)55, (byte)46, (byte)49, (byte)51, (byte)53, (byte)46, (byte)49, (byte)50, (byte)56, (byte)46, (byte)50, (byte)48, (byte)58, (byte)50, (byte)56, (byte)48, (byte)48, (byte)48, (byte)73, (byte)12, (byte)90, (byte)12, (byte)22, (byte)0, (byte)38, (byte)0, (byte)54, (byte)0, (byte)11, (byte)106, (byte)12, (byte)28, (byte)38, (byte)0, (byte)60, (byte)11, (byte)11, (byte)-116, (byte)-104, (byte)12, (byte)-88, (byte)12
				};
			
			
			LoginRsp loginRsp = new LoginRsp();
//			
//			String a = "0A0D0000105ABD2B27BE334FA7A11B72BC40F3C1A31D000030356538396265313665623632623732303035636236356134643437316461396536373762613934363931333436643938224BF23EA039000206143131372E3133352E3132382E31363A323830303006143131372E3133352E3132382E32303A3238303030490C5A0C1600260036000B0B";
//			
//			loginRsp.readFrom(new JceInputStream(HexUtils.convert(a)));
			
			UniPacket packet = new UniPacket();
			packet.setEncodeName("utf-8");
			packet.decode(errorData);
			packet.get(loginRsp, "stLRsp");
//			Enumeration keys =packet.getKeySet();
//			while (keys.hasMoreElements()) {
//				String a = (String) keys.nextElement();
//				System.out.println(a);
//			}
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
