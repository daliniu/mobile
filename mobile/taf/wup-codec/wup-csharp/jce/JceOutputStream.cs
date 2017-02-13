
using Wup.Jce;
using Wup;

using System;
using System.Text;
using System.IO;
using System.Collections.Generic;
using System.Collections;

namespace Wup.Jce
{
    public class JceOutputStream
    {
        private MemoryStream ms;
        private BinaryWriter bw;

        public JceOutputStream(MemoryStream ms)
        {
            this.ms = ms;
            bw = new BinaryWriter(ms, Encoding.BigEndianUnicode);
        }

        public JceOutputStream(int capacity)
        {
            ms = new MemoryStream(capacity);
            bw = new BinaryWriter(ms, Encoding.BigEndianUnicode);
        }

        public JceOutputStream()
        {
            ms = new MemoryStream(128);
            bw = new BinaryWriter(ms, Encoding.BigEndianUnicode);
        }

        public MemoryStream getMemoryStream()
        {
            return ms;
        }

        public byte[] toByteArray()
        {
            byte[] newBytes = new byte[ms.Position];
            System.Array.Copy(ms.GetBuffer(), 0, newBytes, 0, (int)ms.Position);
            return newBytes;
        }

        public void reserve(int len)
        {
            int nRemain = ms.Capacity - (int)ms.Length;
            if (nRemain < len)
            {
                ms.Capacity = (ms.Capacity + len) << 1;// -nRemain;
            }
        }

        // 写入头信息
        public void writeHead(byte type, int tag)
        {
            if (tag < 15)
            {
                byte b = (byte)((tag << 4) | type);

                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(b);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }

            }
            else if (tag < 256)
            {
                try
                {
                    byte b = (byte)((15 << 4) | type);
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(b);
                        bw.Write((byte)tag);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(this + " writeHead: " + e.Message);
                }
            }
            else
            {
                throw new JceEncodeException("tag is too large: " + tag);
            }
        }

        public void Write(bool b, int tag)
        {
            byte by = (byte)(b ? 0x01 : 0);
            Write(by, tag);
        }

        public void Write(byte b, int tag)
        {
            reserve(3);
            if (b == 0)
            {
                writeHead((byte)JceStructType.ZERO_TAG, tag);
            }
            else
            {
                writeHead((byte)JceStructType.BYTE, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(b);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
        }

        public void Write(short n, int tag)
        {
            reserve(4);
            if(n>=-128 && n<=127)// (n >= byte.MinValue && n <= byte.MaxValue)
            {
                Write((byte)n, tag);
            }
            else
            {
                writeHead((byte)JceStructType.SHORT, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(n));
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(this + " Write: " + e.Message);
                }
            }
        }
        public void Write(ushort n, int tag)
        {
            Write((short)n, tag);
        }

        public void Write(int n, int tag)
        {
            reserve(6);

            if (n >= short.MinValue && n <= short.MaxValue)
            {
                Write((short)n, tag);
            }
            else
            {
                writeHead((byte)JceStructType.INT, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(n));
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
        }

        public void Write(uint n, int tag)
        {
            Write((long)n, tag);
        }

        public void Write(ulong n, int tag)
        {
            Write((long)n, tag);
        }

        public void Write(long n, int tag)
        {
            reserve(10);
            if (n >= int.MinValue && n <= int.MaxValue)
            {
                Write((int)n, tag);
            }
            else
            {
                writeHead((byte)JceStructType.LONG, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(n));
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
        }

        public void Write(float n, int tag)
        {
            reserve(6);
            writeHead((byte)JceStructType.FLOAT, tag);
            try
            {
                //using (BinaryWriter bw = new BinaryWriter(ms))
                {
                    bw.Write(ByteConverter.ReverseEndian(n));
                }
            }
            catch (Exception e)
            {
                QTrace.Trace(e.Message);
            }
        }

        public void Write(double n, int tag)
        {
            reserve(10);
            writeHead((byte)JceStructType.DOUBLE, tag);
            try
            {
                //using (BinaryWriter bw = new BinaryWriter(ms))
                {
                    bw.Write(ByteConverter.ReverseEndian(n));
                }
            }
            catch (Exception e)
            {
                QTrace.Trace(e.Message);
            }
        }

        public void writeStringByte(string s, int tag)
        {
            byte[] by = HexUtil.hexStr2Bytes(s);
            reserve(10 + by.Length);	// 预分配足够的缓冲区
            if (by.Length > 255)
            {
                // 长度大于255，为String4类型
                writeHead((byte)JceStructType.STRING4, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(by.Length));
                        bw.Write(by);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
            else
            {
                // 长度小于255，位String1类型
                writeHead((byte)JceStructType.STRING1, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write((byte)by.Length);
                        bw.Write(by);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
        }

        public void writeByteString(string s, int tag)
        {
            reserve(10 + s.Length);
            byte[] by = HexUtil.hexStr2Bytes(s);
            if (by.Length > 255)
            {
                writeHead((byte)JceStructType.STRING4, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(by.Length));
                        bw.Write(by);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
            else
            {
                writeHead((byte)JceStructType.STRING1, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write((byte)by.Length);
                        bw.Write(by);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
        }

        public void Write(string s, int tag,bool IsLocalString = false)
        {
            byte[] by;
            try
            {
                //Encoding en = Encoding.GetEncoding(sServerEncoding);
                //by = en.GetBytes(s);
                by = ByteConverter.String2Bytes(s, IsLocalString);
                if (by == null)
                {
                    by = new byte[0];
                }
            }
            catch (Exception e)
            {
                Wup.QTrace.Trace(this + " write s Exception" + e.Message);
                return;
            }
            if (by != null)
            {
                reserve(10 + by.Length);
            }
            if (by != null && by.Length > 255)
            {
                writeHead((byte)JceStructType.STRING4, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        bw.Write(ByteConverter.ReverseEndian(by.Length));
                        bw.Write(by);
                    }
                }
                catch (Exception e)
                {
                    QTrace.Trace(e.Message);
                }
            }
            else
            {
                writeHead((byte)JceStructType.STRING1, tag);
                try
                {
                    //using (BinaryWriter bw = new BinaryWriter(ms))
                    {
                        if (by != null)
                        {
                            bw.Write((byte)by.Length);
                            bw.Write(by);
                        }
                        else
                        {
                            bw.Write((byte)0);
                        }
                    }
                }
                catch (Exception e)
                {
                    Wup.QTrace.Trace(this + " write s(2) Exception" + e.Message);
                }
            }
        }

        public void write<K, V>(Dictionary<K, V> m, int tag)
        {
            reserve(8);
            writeHead((byte)JceStructType.MAP, tag);
            
            Write(m == null ? 0 : m.Count, 0);
            if (m != null)
            {
                foreach (KeyValuePair<K, V> en in m)
                {
                    Write(en.Key, 0);
                    Write(en.Value, 1);
                }
            }
        }

        public void Write(IDictionary m, int tag)
        {
            reserve(8);
            writeHead((byte)JceStructType.MAP, tag);
            Write(m == null ? 0 : m.Count, 0);
            if (m != null)
            {
                ICollection keys = m.Keys;
                foreach (object objKey in keys)
                {
                    Write(objKey, 0);
                    Write(m[objKey], 1);
                }
            }
        }

        public void Write(bool[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (bool e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(byte[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8 + nLen);
            writeHead((byte)JceStructType.SIMPLE_LIST, tag);
            writeHead((byte)JceStructType.BYTE, 0);
            Write(nLen, 0);

            try
            {
                //using (BinaryWriter bw = new BinaryWriter(ms))
                if(l != null)
                {
                    bw.Write(l);
                }
            }
            catch (Exception e)
            {
                QTrace.Trace(e.Message);
            }
        }

        public void Write(short[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (short e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(int[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (int e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(long[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (long e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(float[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (float e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(double[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (double e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void write<T>(T[] l, int tag)
        {
            object o = l;
            writeArray((object[])o, tag);
        }

        private void writeArray(object[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (Object e in l)
                {
                    Write(e, 0);
                }
            }
        }

        // 由于list，应该在第一个位置[0]预置一个元素（可以为空），以方便判断元素类型
        public void writeList(IList l, int tag)
        {
            reserve(8);
            writeHead((byte)JceStructType.LIST, tag);
            Write(l == null ? 0 : (l.Count>0?l.Count:0), 0);
           // Write(l == null ? 0 : l.Count, 0);
            if (l != null)
            {
                //foreach (T e in l)
                for(int i = 0; i<l.Count; i++)
                {
                    Write(l[i], 0);
                }
            }
        }

        public void Write(JceStruct o, int tag)
        {
            if (o == null)
            {
                return;
            }
            reserve(2);
            writeHead((byte)JceStructType.STRUCT_BEGIN, tag);
            o.WriteTo(this);
            reserve(2);
            writeHead((byte)JceStructType.STRUCT_END, 0);
        }

        public void Write(object o, int tag)
        {
            if (o == null)
            {
                return;
            }
            if (o is byte)
            {
                Write(((byte)o), tag);
            }
            else if (o is Boolean)
            {
                Write((bool)o, tag);
            }
            else if (o is short)
            {
                Write(((short)o), tag);
            }
            else if (o is ushort)
            {
                Write(((int)(ushort)o), tag);
            }
            else if (o is int)
            {
                Write(((int)o), tag);
            }
            else if (o is uint)
            {
                Write((long)(uint)o, tag);
            }
            else if (o is long)
            {
                Write(((long)o), tag);
            }
            else if (o is ulong)
            {
                Write(((long)(ulong)o), tag);
            }
            else if (o is float)
            {
                Write(((float)o), tag);
            }
            else if (o is double)
            {
                Write(((double)o), tag);
            }
            else if (o is string)
            {
                string strObj = o as string;
                Write(strObj, tag);
            }
            else if (o is JceStruct)
            {
                Write((JceStruct)o, tag);
            }
            else if (o is byte[])
            {
                Write((byte[])o, tag);
            }
            else if (o is bool[])
            {
                Write((bool[])o, tag);
            }
            else if (o is short[])
            {
                Write((short[])o, tag);
            }
            else if (o is int[])
            {
                Write((int[])o, tag);
            }
            else if (o is long[])
            {
                Write((long[])o, tag);
            }
            else if (o is float[])
            {
                Write((float[])o, tag);
            }
            else if (o is double[])
            {
                Write((double[])o, tag);
            }
            else if (o.GetType().IsArray)
            {
                Write((object[])o, tag);
            }
            else if (o is IList)
            {
                writeList((IList)o, tag);
            }
            else if (o is IDictionary)
            {
                Write((IDictionary)o, tag);
            }
            else
            {
                throw new JceEncodeException(
                        "write object error: unsupport type. " + o.ToString() + "\n");
            }
        }

        protected string sServerEncoding = "UTF-8";
        public int setServerEncoding(string se)
        {
            sServerEncoding = se;
            return 0;
        }
    }
}