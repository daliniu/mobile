package 
{
	import flash.utils.ByteArray;
	import com.qq.MTT.*;
	import com.qq.taf.jce.*;
	public class demo
	{
		public static function TestA(str1:String="",str2:String=""):String
		{
			var ost:ByteArray = new ByteArray();
			var a:A = new A();
			//encode
			a.nId = 65;
			a.sMsg = str1;
			a.sNew = str2;
			
			a.vNews.push("a");
			a.vNews.push("b");
			a.vNews.push("c");
			a.vNews.push("d");
			A.writeTo(ost,a);
			
			//decode
			var ae:A = A.readFrom(getByteArray(ost));
			var result:String = "sMsg: " + ae.sMsg + "\n" + "sNew:" + ae.sNew + "\n";
			
			trace(result);
			
			return result;
		}
		
		public static function TestB(str1:String="",str2:String=""):String
		{
			var ost:ByteArray = new ByteArray();
            var value:B = new B();
			
            value.nId = 67;
            value.sName= "Test B";
            value.vFavor.push(str1);
			value.vFavor.push(str2);
            value.ePet= 1;
            value.vByte.writeUTF("vector<byte>"); 
            value.sPhone = "123456";
            value.bMan= true;
			value.stA.sMsg = str1;
			value.stA.sNew = str2;
			
			B.writeTo(ost,value);
			
			//decode
			var b:B = B.readFrom(getByteArray(ost));
			var result:String = "sName: " + b.sName + "\n" + "sPhone:" + b.sPhone + "\n" 
								+ "vFavor:{" + b.vFavor.toString() +"}\n" +
								"a.sMsg: " + b.stA.sMsg + "\n" + "a.sNew:" + b.stA.sNew + "\n";;
			trace(result);
			return result;
		}
		
		public static function getByteArray(byts:ByteArray):ByteArray
		{
			byts.position = 0;
			var ba:ByteArray = new ByteArray();
			byts.readBytes(ba,0,byts.bytesAvailable);
			ba.position = 0;
			return ba;
		}
	}
}