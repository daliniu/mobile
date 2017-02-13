using System.IO;
using System;
using System.Collections.Generic;
using Wup;
using System.Text;
using System.Collections;

namespace Wup.Jce
{
    /**
     * 数据读取流
     * @author meteorchen
     *
     */
    public class JceInputStream
    {
        // private ByteBuffer bs;	// 缓冲区

        private MemoryStream ms;
        private BinaryReader br;

        /**
         * 头数据
         * @author meteorchen
         *
         */
        public class HeadData
        {

            public byte type;
            public int tag;

            public void clear()
            {
                type = 0;
                tag = 0;
            }
        }

        public JceInputStream()
        {
            this.ms = new MemoryStream();
            br = null;
            br = new BinaryReader(ms);
        }


        public JceInputStream(MemoryStream ms)
        {
            this.ms = ms;
            br = null;
            br = new BinaryReader(ms);
        }

        public JceInputStream(byte[] bs)
        {
            this.ms = new MemoryStream(bs);
            br = null;
            br = new BinaryReader(ms);
        }

        public JceInputStream(byte[] bs, int pos)
        {
            this.ms = new MemoryStream(bs);
            ms.Position = pos;
            br = null;
            br = new BinaryReader(ms);
        }

        public void wrap(byte[] bs, int index = 0)
        {
            if (null != this.ms)
            {
                ms = null;
                this.ms = new MemoryStream(bs, index, bs.Length-index);
                br = null;
                br = new BinaryReader(ms);
            }
            else
            {
                this.ms = new MemoryStream(bs);
                br = null;
                br = new BinaryReader(ms);
            }
        }

        /**
         * 读取数据头
         * @param hd	读取到的头信息
         * @param bb	缓冲
         * @return 读取的字节数
         */
        public static int readHead(HeadData hd, BinaryReader bb)
        {
			if(bb.BaseStream.Position >= bb.BaseStream.Length)
			{
				throw new JceDecodeException("read file to end");
			}
            byte b = bb.ReadByte();
            hd.type = (byte)(b & 15);
            hd.tag = ((b & (15 << 4)) >> 4);
            if (hd.tag == 15)
            {
                hd.tag = bb.ReadByte();
                return 2;
            }
            return 1;
        }

        public int readHead(HeadData hd)
        {
            int nLen = 0;
            //using (BinaryReader br = new BinaryReader(ms))
            {
                nLen = readHead(hd, br);
            }
            return nLen;
        }

        // 读取头信息，但不移动缓冲区的当前偏移
        private int peakHead(HeadData hd)
        {
            long curPos = ms.Position;
            int len = readHead(hd);
            ms.Position = curPos;
            return len;
        }

        // 跳过若干字节
        private void skip(int len)
        {
            ms.Position += len;
        }

        // 跳到指定的tag的数据之前
        public bool skipToTag(int tag)
        {
            try
            {
                HeadData hd = new HeadData();
                while (true)
                {
                    int len = peakHead(hd);
                    if (tag <= hd.tag || hd.type == (byte)JceStructType.STRUCT_END)
                    {
                        return tag == hd.tag;
                    }

                    skip(len);
                    skipField(hd.type);
                }
            }
            catch (JceDecodeException e)
            {
                QTrace.Trace(e.Message);
            }
            return false;
        }

        // 跳到当前结构的结束位置
        public void skipToStructEnd()
        {
            HeadData hd = new HeadData();
            do
            {
                readHead(hd);
                skipField(hd.type);
            } while (hd.type != (byte)JceStructType.STRUCT_END);
        }

        // 跳过一个字段
        private void skipField()
        {
            HeadData hd = new HeadData();
            readHead(hd);
            skipField(hd.type);
        }

        private void skipField(byte type)
        {
            switch (type)
            {
                case (byte)JceStructType.BYTE:
                    skip(1);
                    break;
                case (byte)JceStructType.SHORT:
                    skip(2);
                    break;
                case (byte)JceStructType.INT:
                    skip(4);
                    break;
                case (byte)JceStructType.LONG:
                    skip(8);
                    break;
                case (byte)JceStructType.FLOAT:
                    skip(4);
                    break;
                case (byte)JceStructType.DOUBLE:
                    skip(8);
                    break;
                case (byte)JceStructType.STRING1:
                    {
                        // BinaryReader br = new BinaryReader(ms);

                        int len = br.ReadByte();
                        if (len < 0)
                            len += 256;
                        skip(len);
                        break;
                    }
                case (byte)JceStructType.STRING4:
                    {
                        //  BinaryReader br = new BinaryReader(ms);
                        skip(ByteConverter.ReverseEndian(br.ReadInt32()));
                        break;
                    }
                case (byte)JceStructType.MAP:
                    {
                        int size = Read(0, 0, true);
                        for (int i = 0; i < size * 2; ++i)
                        {
                            skipField();
                        }
                        break;
                    }
                case (byte)JceStructType.LIST:
                    {
                        int size = Read(0, 0, true);
                        for (int i = 0; i < size; ++i)
                        {
                            skipField();
                        }
                        break;
                    }
                case (byte)JceStructType.SIMPLE_LIST:
                    {
                        HeadData hd = new HeadData();
                        readHead(hd);
                        if (hd.type != (byte)JceStructType.BYTE)
                        {
                            throw new JceDecodeException("skipField with invalid type, type value: " + type + ", " + hd.type);
                        }
                        int size = Read(0, 0, true);
                        skip(size);
                        break;
                    }
                case (byte)JceStructType.STRUCT_BEGIN:
                    skipToStructEnd();
                    break;
                case (byte)JceStructType.STRUCT_END:
                case (byte)JceStructType.ZERO_TAG:
                    break;
                default:
                    throw new JceDecodeException("invalid type.");
            }
        }

        public bool Read(bool b, int tag, bool isRequire)
        {
            byte c = Read((byte)0x0, tag, isRequire);
            return c != 0;
        }

        public char Read(char c, int tag, bool isRequire)
        {
            return (char)Read((byte)c, tag, isRequire);
        }

        public byte Read(byte c, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        c = 0x0;
                        break;
                    case (byte)JceStructType.BYTE:
                        {
                            // BinaryReader br = new BinaryReader(ms);
                            c = br.ReadByte();
                            break;
                        }
                    default:
                        {
                            throw new JceDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return c;
        }

        public short Read(short n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        {
                            //BinaryReader br = new BinaryReader(ms);
                            n = (System.SByte)br.ReadByte();
                            break;
                        }
                    case (byte)JceStructType.SHORT:
                        {
                            //BinaryReader br = new BinaryReader(ms);
                            n = ByteConverter.ReverseEndian(br.ReadInt16());
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public ushort Read(ushort n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        {
                            n = br.ReadByte();
                            break;
                        }
                    case (byte)JceStructType.SHORT:
                        {
                            n = ByteConverter.ReverseEndian(br.ReadUInt16());
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public int Read(int n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                

                //BinaryReader br = new BinaryReader(ms);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        n = (System.SByte)br.ReadByte();
                        break;
                    case (byte)JceStructType.SHORT:
                        n = ByteConverter.ReverseEndian(br.ReadInt16());
                        break;
                    case (byte)JceStructType.INT:
                        n = ByteConverter.ReverseEndian(br.ReadInt32());
                        break;
                    case (byte)JceStructType.LONG:
                        n = (int)ByteConverter.ReverseEndian(br.ReadInt32());
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public uint Read(uint n, int tag, bool isRequire)
        {
            return (uint)Read((long)n, tag, isRequire);
            /*
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);

                //BinaryReader br = new BinaryReader(ms);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        n = br.ReadByte();
                        break;
                    case (byte)JceStructType.SHORT:
                        n = ByteConverter.ReverseEndian(br.ReadUInt16());
                        break;
                    case (byte)JceStructType.INT:
                        n = ByteConverter.ReverseEndian(br.ReadUInt32());
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
             */
        }

        public long Read(long n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = (System.SByte)br.ReadByte();
                        }
                        break;
                    case (byte)JceStructType.SHORT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadInt16());
                        }
                        break;
                    case (byte)JceStructType.INT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadInt32());
                        }
                        break;
                    case (byte)JceStructType.LONG:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadInt64());
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public ulong Read(ulong n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.BYTE:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = br.ReadByte();
                        }
                        break;
                    case (byte)JceStructType.SHORT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadUInt16());
                        }
                        break;
                    case (byte)JceStructType.INT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadUInt32());
                        }
                        break;
                    case (byte)JceStructType.LONG:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadUInt64());
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }
        public float Read(float n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.FLOAT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadSingle());
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public double Read(double n, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.ZERO_TAG:
                        n = 0;
                        break;
                    case (byte)JceStructType.FLOAT:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadSingle());
                        }
                        break;
                    case (byte)JceStructType.DOUBLE:
                        //using (BinaryReader br = new BinaryReader(ms))
                        {
                            n = ByteConverter.ReverseEndian(br.ReadDouble());
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return n;
        }

        public string readByteString(string s, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.STRING1:
                        {
                            //using (BinaryReader br = new BinaryReader(ms))
                            {
                                int len = 0;
                                len = br.ReadByte();
                                if (len < 0)
                                {
                                    len += 256;
                                }

                                byte[] ss = new byte[len];
                                ss = br.ReadBytes(len);
                                s = HexUtil.bytes2HexStr(ss);
                            }
                        }
                        break;
                    case (byte)JceStructType.STRING4:
                        {
                            //using (BinaryReader br = new BinaryReader(ms))
                            {
                                int len = 0;
                                len = ByteConverter.ReverseEndian(br.ReadInt32());

                                if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
                                {
                                    throw new JceDecodeException("string too long: " + len);
                                }

                                byte[] ss = new byte[len];
                                ss = br.ReadBytes(len);
                                s = HexUtil.bytes2HexStr(ss);
                            }
                        }
                        break;
                    default:
                        {
                            throw new JceDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return s;
        }

        private string readString1()
        {
            //using (BinaryReader br = new BinaryReader(ms))
            {
                int len = 0;
                len = br.ReadByte();
                if (len < 0)
                {
                    len += 256;
                }

                byte[] ss = new byte[len];
                ss = br.ReadBytes(len);

                //string strUtf8 = Encoding.UTF8.GetString(ss, 0, ss.Length);

                //string strUnicode = Encoding.Unicode.GetString(ss, 0, ss.Length);

                return ByteConverter.Bytes2String(ss);
                //Encoding en =Encoding.GetEncoding(sServerEncoding);
                //return en.GetString(ss, 0, ss.Length);
                /*
                char[] chs = Encoding.UTF8.GetChars(ss);
                char[] chsuni = Encoding.Unicode.GetChars(ss);
                char[] chsbig = Encoding.BigEndianUnicode.GetChars(ss);

                return Encoding.UTF8.GetString(ss, 0, ss.Length);
                 */
                //return BitConverter.ToString(ss);
            }//
        }

        private string readString4()
        {
            //using (BinaryReader br = new BinaryReader(ms))
            {
                int len = 0;
                len = ByteConverter.ReverseEndian(br.ReadInt32());
                if (len > JceStruct.JCE_MAX_STRING_LENGTH || len < 0)
                {
                    throw new JceDecodeException("string too long: " + len);
                }

                byte[] ss = new byte[len];
                ss = br.ReadBytes(len);

                //Encoding en = Encoding.GetEncoding(sServerEncoding);
                //return en.GetString(ss, 0, ss.Length);

                return ByteConverter.Bytes2String(ss);
            }
        }

        public string Read(string s, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.STRING1:
                        {
                            s = readString1();
                        }
                        break;
                    case (byte)JceStructType.STRING4:
                        {
                            s = readString4();
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return s;
        }

        public string readString(int tag, bool isRequire)
        {
            string s = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.STRING1:
                        {
                            s = readString1();
                        }
                        break;
                    case (byte)JceStructType.STRING4:
                        {
                            s = readString4();
                        }
                        break;
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return s;
        }

        public string[] Read(string[] s, int tag, bool isRequire)
        {
            return readArray(s, tag, isRequire);
        }

        public IDictionary readMap<T>(int tag, bool isRequire)
        {
            T m = (T)BasicClassTypeUtil.CreateObject<T>();
            return readMap<T>(m, tag, isRequire);
        }
        public IDictionary readMap<T>(T arg, int tag, bool isRequire)
        {
            IDictionary m = BasicClassTypeUtil.CreateObject(arg.GetType()) as IDictionary;
            if (m == null)
            {
                //throw new JceDecodeException("Creat Object Fail...");
                return null;
            }

            Type type = m.GetType();
            Type[] argsType = type.GetGenericArguments();
            if (argsType == null || argsType.Length < 2)
            {
                return null;
            }

            var mk = BasicClassTypeUtil.CreateObject(argsType[0]);
            var mv = BasicClassTypeUtil.CreateObject(argsType[1]);

            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.MAP:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new JceDecodeException("size invalid: " + size);
                            }
                            for (int i = 0; i < size; ++i)
                            {
                                mk = Read(mk, 0, true);
                                mv = Read(mv, 1, true);

                                if (mk != null)
                                {
                                    if (m.Contains(mk))
                                    {
                                        m[mk] = mv;
                                    }
                                    else
                                    {
                                        m.Add(mk, mv);
                                    }
                                }

                            }
                        }
                        break;
                    default:
                        {
                            throw new JceDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return m;
        }

        /*
        public Dictionary<K, V> readMap<K, V>(Dictionary<K, V> m, int tag, bool isRequire)
        {
            return (Dictionary<K, V>)readMap(new Dictionary<K, V>(), m, tag, isRequire);
        }

        ////@SuppressWarnings("unchecked")
        private Dictionary<K, V> readMap<K, V>(Dictionary<K, V> mr, Dictionary<K, V> m, int tag, bool isRequire)
        {
            if (m == null || m.Count == 0)
            {
                //throw new TafProxyDecodeException("unable to get type of key and value.");
                return new Dictionary<K, V>();
            }

            // 生成代码时已经往map里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到map里面放的是什么类型的数据

            KeyValuePair<K, V> pair = m.GetEnumerator().Current;
            K mk = pair.Key;
            V mv = pair.Value;

            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.MAP:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new JceDecodeException("size invalid: " + size);
                            }
                            for (int i = 0; i < size; ++i)
                            {
                                K k = (K)Read(mk, 0, true);
                                V v = (V)Read(mv, 1, true);
                                mr.Add(k, v);
                            }
                        }
                        break;
                    default:
                        {
                            throw new JceDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return mr;
        }
*/

        /*
        ////@SuppressWarnings("unchecked")
        public Array readList(int tag, bool isRequire) 
        {
            Array lr = new Array();
            if (skipToTag(tag)) {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type) {
                case JceStruct.LIST: {
                    int size = Read(0, 0, true);
                    if (size < 0)
                        throw new JceDecodeException("size invalid: " + size);
                    for (int i = 0; i < size; ++i) {
                        HeadData subH = new HeadData();
                        readHead(subH);
                        switch (subH.type) {
                        case JceStruct.BYTE:
                            skip(1);
                            break;
                        case JceStruct.SHORT:
                            skip(2);
                            break;
                        case JceStruct.INT:
                            skip(4);
                            break;
                        case JceStruct.LONG:
                            skip(8);
                            break;
                        case JceStruct.FLOAT:
                            skip(4);
                            break;
                        case JceStruct.DOUBLE:
                            skip(8);
                            break;
                        case JceStruct.STRING1: {
                            int len = bs.Get();
                            if (len < 0)
                                len += 256;
                            skip(len);
                        }
                            break;
                        case JceStruct.STRING4: {
                            skip(bs.getInt());
                        }
                            break;
                        case JceStruct.MAP: {

                        }
                            break;
                        case JceStruct.LIST: {

                        }
                            break;
                        case JceStruct.STRUCT_BEGIN:
                            try {
                                Class<?> newoneClass = Class.forName(JceStruct.class.getName());
                                Constructor<?> cons = newoneClass.getConstructor();
                                JceStruct struct = (JceStruct) cons.newInstance();
                                struct.ReadFrom(this);
                                skipToStructEnd();
                                lr.add(struct);
                            } catch (Exception e) {
                                e.printStackTrace();
                                throw new JceDecodeException("type mismatch." + e);
                            }
                            break;
                        case JceStruct.ZERO_TAG:
                            lr.add(new int(0));
                            break;
                        default:
                            throw new JceDecodeException("type mismatch.");
                        }
                        // T t = Read(mt, 0, true);
                        // lr.add(t);
                    }
                }
                    break;
                default:
                    throw new JceDecodeException("type mismatch.");
                }
            } else if (isRequire) {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }
        */
        public bool[] Read(bool[] l, int tag, bool isRequire)
        {
            bool[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new bool[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public byte[] Read(byte[] l, int tag, bool isRequire)
        {
            byte[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.SIMPLE_LIST:
                        {
                            HeadData hh = new HeadData();
                            readHead(hh);
                            if (hh.type != (byte)JceStructType.BYTE)
                            {
                                throw new JceDecodeException("type mismatch, tag: " + tag + ", type: " + hd.type + ", " + hh.type);
                            }

                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new JceDecodeException("invalid size, tag: " + tag + ", type: " + hd.type + ", " + hh.type + ", size: " + size);
                            }

                            lr = new byte[size];


                            try
                            {

                                //BinaryReader br = new BinaryReader(ms);
                                lr = br.ReadBytes(size);
                            }
                            catch (Exception e)
                            {
                                QTrace.Trace(e.Message);
                                return null;
                            }
                            break;
                        }
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new byte[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public short[] Read(short[] l, int tag, bool isRequire)
        {
            short[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new short[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public int[] Read(int[] l, int tag, bool isRequire)
        {
            int[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new int[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public long[] Read(long[] l, int tag, bool isRequire)
        {
            long[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new long[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public float[] Read(float[] l, int tag, bool isRequire)
        {
            float[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new float[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public double[] Read(double[] l, int tag, bool isRequire)
        {
            double[] lr = null;
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new JceDecodeException("size invalid: " + size);
                            lr = new double[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new JceDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return lr;
        }

        public T[] readArray<T>(T[] l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null || l.Length == 0)
                throw new JceDecodeException("unable to get type of key and value.");
            return (T[])readArrayImpl(l[0], tag, isRequire);
        }

        public IList readList<T>(T l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null)
            {
                return null;
            }

            IList list = BasicClassTypeUtil.CreateObject(l.GetType()) as IList;
            if (list == null)
            {
                return null;
            }

            object objItem = BasicClassTypeUtil.CreateListItem(list.GetType());

            Array array = readArrayImpl(objItem, tag, isRequire);

            if (array != null)
            {
                list.Clear();
                foreach (object obj in array)
                {
                    list.Add(obj);
                }

                return list;
            }

            return null;
        }

        public List<T> readArray<T>(List<T> l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null || l.Count == 0)
            {
                return new List<T>();
                //throw new TafProxyDecodeException("unable to get type of key and value.");
            }
            T[] tt = (T[])readArrayImpl(l[0], tag, isRequire);
            if (tt == null) return null;
            List<T> ll = new List<T>();
            for (int i = 0; i < tt.Length; ++i)
            {
                ll.Add(tt[i]);
            }
            return ll;
        }

        ////@SuppressWarnings("unchecked")
        private Array readArrayImpl<T>(T mt, int tag, bool isRequire)
        {
            if (skipToTag(tag))
            {
                HeadData hd = new HeadData();
                readHead(hd);
                //byte type = 0;

                //if (mt.GetType() == type.GetType())
                //{
                //    hd.type = (byte)JceStructType.SIMPLE_LIST;
                //}
                switch (hd.type)
                {
                    case (byte)JceStructType.LIST:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new JceDecodeException("size invalid: " + size);
                            }

                            Array lr = Array.CreateInstance(mt.GetType(), size);
                            for (int i = 0; i < size; ++i)
                            {
                                T t = (T)Read(mt, 0, true);
                                lr.SetValue(t, i);
                            }
                            return lr;
                        }

                    case (byte)JceStructType.SIMPLE_LIST:
                        {
                            HeadData hh = new HeadData();
                            readHead(hh);
                            if (hh.type == (byte)JceStructType.ZERO_TAG)
                            {
                                return Array.CreateInstance(mt.GetType(), 0);
                            }
                            if (hh.type != (byte)JceStructType.BYTE)
                            {
                                throw new JceDecodeException("type mismatch, tag: " + tag + ", type: " + hd.type + ", " + hh.type);
                            }
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new JceDecodeException("invalid size, tag: " + tag + ", type: " + hd.type + ", size: " + size);
                            }

                            T[] lr = new T[size];

                            try
                            {
                                byte[] lrtmp = br.ReadBytes(size);
                                for (int i = 0; i < lrtmp.Length; i++)
                                {
                                    object obj = lrtmp[i];
                                    lr[i] = (T)obj;
                                }

                                return lr;
                            }
                            catch (Exception e)
                            {
                                QTrace.Trace(e.Message);
                                return null;
                            }
                            //break;
                        }
                    default:
                        {
                            throw new JceDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return null;
        }
        
        public JceStruct directRead(JceStruct o, int tag, bool isRequire)
        {
            //o必须有一个无参的构造函数
            JceStruct reff = null;
            if (skipToTag(tag))
            {
                try
                {
                    reff = (JceStruct)BasicClassTypeUtil.CreateObject(o.GetType());
                }
                catch (Exception e)
                {
                    throw new JceDecodeException(e.Message);
                }

                HeadData hd = new HeadData();
                readHead(hd);
                if (hd.type != (byte)JceStructType.STRUCT_BEGIN)
                {
                    throw new JceDecodeException("type mismatch.");
                }
                reff.ReadFrom(this);
                skipToStructEnd();
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }

            return reff;
        }

        public JceStruct Read(JceStruct o, int tag, bool isRequire)
        {
            //o必须有一个无参的构造函数
            JceStruct reff = null;
            if (skipToTag(tag))
            {
                try
                {
                    // 必须重新创建一个，否则，会导致在同一个对象上赋值，这是由于C#的引用引起的
                    reff = (JceStruct)BasicClassTypeUtil.CreateObject(o.GetType());
                }
                catch (Exception e)
                {
                    throw new JceDecodeException(e.Message);
                }

                HeadData hd = new HeadData();
                readHead(hd);
                if (hd.type != (byte)JceStructType.STRUCT_BEGIN)
                {
                    throw new JceDecodeException("type mismatch.");
                }
                reff.ReadFrom(this);
                skipToStructEnd();
            }
            else if (isRequire)
            {
                throw new JceDecodeException("require field not exist.");
            }
            return reff;
        }

        public JceStruct[] Read(JceStruct[] o, int tag, bool isRequire)
        {
            return readArray(o, tag, isRequire);
        }

        ////@SuppressWarnings("unchecked")
        public object Read<T>(T o, int tag, bool isRequire)
        {
            if (o == null)
            {
                o = (T)BasicClassTypeUtil.CreateObject<T>();
            }
            if (o is Byte || o is Char)
            {
                return (Read((byte)0x0, tag, isRequire));
            }
            else if (o is char)
            {
                return (Read((char)0x0, tag, isRequire));
            }
            else if (o is Boolean)
            {
                return (Read(false, tag, isRequire));
            }
            else if (o is short)
            {
                return (Read((short)0, tag, isRequire));
            }
            else if (o is ushort)
            {
                return (Read((ushort)0, tag, isRequire));
            }
            else if (o is int)
            {
                return Read((int)0, tag, isRequire);
            }
            else if (o is uint)
            {
                return Read((uint)0, tag, isRequire);
            }
            else if (o is long)
            {
                return (Read((long)0, tag, isRequire));
            }
            else if (o is ulong)
            {
                return (Read((ulong)0, tag, isRequire));
            }
            else if (o is float)
            {
                return (Read((float)0, tag, isRequire));
            }
            else if (o is Double)
            {
                return (Read((double)0, tag, isRequire));
            }
            else if (o is string)
            {
                return (readString(tag, isRequire));
            }
            else if (o is JceStruct)
            {
                object oo = o;
                return Read((JceStruct)oo, tag, isRequire);
            }
            else if (o != null && o.GetType().IsArray)
            {
                if (o is byte[] || o is Byte[])
                {
                    return Read((byte[])null, tag, isRequire);
                }
                else if (o is bool[])
                {
                    return Read((bool[])null, tag, isRequire);
                }
                else if (o is short[])
                {
                    return Read((short[])null, tag, isRequire);
                }
                else if (o is int[])
                {
                    return Read((int[])null, tag, isRequire);
                }
                else if (o is long[])
                {
                    return Read((long[])null, tag, isRequire);
                }
                else if (o is float[])
                {
                    return Read((float[])null, tag, isRequire);
                }
                else if (o is double[])
                {
                    return Read((double[])null, tag, isRequire);
                }
                else
                {
                    object oo = o;
                    return readArray((Object[])oo, tag, isRequire);
                }
            }
            else if (o is IList)
            {
                return readList<T>(o, tag, isRequire);
            }
            else if (o is IDictionary)
            {
                return readMap<T>(o, tag, isRequire);
            }
            else
            {
                throw new JceDecodeException("read object error: unsupport type." + o.ToString());
            }
        }

        protected string sServerEncoding = "utf-8";
        public int setServerEncoding(string se)
        {
            sServerEncoding = se;
            return 0;
        }

        internal object read(object proxy, int tag, bool isRequired)
        {
            return Read(proxy, tag, isRequired);
        }
    }
}