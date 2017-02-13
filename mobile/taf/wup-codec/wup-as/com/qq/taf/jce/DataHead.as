package com.qq.taf.jce
{
    import flash.utils.ByteArray;
    
    public class DataHead
    {
        public static const EM_INT8         :int =  0;
        public static const EM_INT16        :int =  1;
        public static const EM_INT32        :int =  2;
        public static const EM_INT64        :int =  3;
        public static const EM_FLOAT        :int =  4;
        public static const EM_DOUBLE       :int =  5;
        public static const EM_STRING1      :int =  6; 
        public static const EM_STRING4      :int =  7;
        public static const EM_MAP          :int =  8;
        public static const EM_LIST         :int =  9;
        public static const EM_STRUCTBEGIN  :int = 10;
        public static const EM_STRUCTEND    :int = 11;
        public static const EM_ZERO         :int = 12;
        public static const EM_SIMPLELIST   :int = 13;
        
        public function readFrom(ist:ByteArray):void
        {
            var tmp:int = ist.readByte();
            
            _tag     = (tmp & 0xF0) >> 4;
            _type    = (tmp & 0x0F);
            if (_tag == 15) 
			{
				_tag = ist.readByte();
			}
        }

        public function peekFrom(ist:ByteArray):int
        {
            var len:int  = 1;
            var pos:uint = ist.position;
            
            readFrom(ist);
            
            if (_tag >= 15)
            {
                len = 2;
            }

            ist.position = pos;
            return len;
        }
        
        public static function writeTo(ost:ByteArray, tag:int, type:int):void
        {
            if (tag < 15)
            {
                ost.writeByte(((tag << 4) & 0xF0) | (type & 0x0F));
            }
            else
            {
                ost.writeByte(((15 << 4) & 0xF0) | (type & 0x0F));
                ost.writeByte(tag);
            }
        }
        
        public var _tag:int = 0;
        public var _type:int = 0;
    };
}