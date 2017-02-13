
using System;
using System.Text;
using System.Collections.Generic;
using System.Collections;
using Wup;

namespace Wup.Jce
{
    /**
     * 格式化输出jce结构的所有属性
     * 主要用于调试或打日志
     * @author meteorchen
     *
     */
    public class JceDisplayer
    {
        private StringBuilder sb;
        private int _level = 0;

        private void ps(string fieldName)
        {
            for (int i = 0; i < _level; ++i)
            {
                sb.Append('\t');
            }

            if (fieldName != null)
            {
                sb.Append(fieldName).Append(": ");
            }
        }

        public JceDisplayer(StringBuilder sb, int level)
        {
            this.sb = sb;
            this._level = level;
        }

        public JceDisplayer(StringBuilder sb)
        {
            this.sb = sb;
        }

        public JceDisplayer Display(bool b, string fieldName)
        {
            ps(fieldName);
            sb.Append(b ? 'T' : 'F').Append('\n');
            return this;
        }

        public JceDisplayer Display(byte n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(char n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(short n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(int n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(long n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(float n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(double n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public JceDisplayer Display(string s, string fieldName)
        {
            ps(fieldName);
            if (null == s)
            {
                sb.Append("null").Append('\n');
            }
            else
            {
                sb.Append(s).Append('\n');
            }

            return this;
        }

        public JceDisplayer Display(byte[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (byte o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(char[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (char o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(short[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (short o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(int[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (int o in v)
                jd.Display(o, null);
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(long[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (long o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(float[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (float o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display(double[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (double o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display<K, V>(Dictionary<K, V> m, string fieldName)
        {
            ps(fieldName);
            if (null == m)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (m.Count == 0)
            {
                sb.Append(m.Count).Append(", {}").Append('\n');
                return this;
            }
            sb.Append(m.Count).Append(", {").Append('\n');
            JceDisplayer jd1 = new JceDisplayer(sb, _level + 1);
            JceDisplayer jd = new JceDisplayer(sb, _level + 2);
            foreach (KeyValuePair<K, V> en in m)
            {
                jd1.Display('(', null);
                jd.Display(en.Key, null);
                jd.Display(en.Value, null);
                jd1.Display(')', null);
            }
            Display('}', null);
            return this;
        }

        public JceDisplayer Display<T>(T[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            JceDisplayer jd = new JceDisplayer(sb, _level + 1);
            foreach (T o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public JceDisplayer Display<T>(List<T> v, string fieldName)
        {
            if (null == v)
            {
                ps(fieldName);
                //sb.Append("null").Append('\t');
                sb.Append("null").Append('\n');
                return this;
            }
            else
            {
                for (int i = 0; i < v.Count; i++)
                {
                    Display(v[i], fieldName);
                }
                return this;
            }
        }

        ////@SuppressWarnings("unchecked")
        public JceDisplayer Display<T>(T o, string fieldName)
        {
            object oObject = o;

            if (null == o)
            {
                sb.Append("null").Append('\n');
            }

            else if (o is byte)
            {
                Display(((byte)oObject), fieldName);
            }
            else if (o is bool)
            {
                Display(((bool)oObject), fieldName);
            }
            else if (o is short)
            {
                Display(((short)oObject), fieldName);
            }
            else if (o is int)
            {
                Display(((int)oObject), fieldName);
            }
            else if (o is long)
            {
                Display(((long)oObject), fieldName);
            }
            else if (o is float)
            {
                Display(((float)oObject), fieldName);
            }
            else if (o is Double)
            {
                Display(((Double)oObject), fieldName);
            }
            else if (o is string)
            {
                Display((string)oObject, fieldName);
            }
            else if (o is JceStruct)
            {
                Display((JceStruct)oObject, fieldName);
            }
            else if (o is byte[])
            {
                Display((byte[])oObject, fieldName);
            }
            else if (o is bool[])
            {
                Display((bool[])oObject, fieldName);
            }
            else if (o is short[])
            {
                Display((short[])oObject, fieldName);
            }
            else if (o is int[])
            {
                Display((int[])oObject, fieldName);
            }
            else if (o is long[])
            {
                Display((long[])oObject, fieldName);
            }
            else if (o is float[])
            {
                Display((float[])oObject, fieldName);
            }
            else if (o is double[])
            {
                Display((double[])oObject, fieldName);
            }
            else if (o.GetType().IsArray)
            {
                Display((Object[])oObject, fieldName);
            }
            else if (o is IList)
            {
                IList list = (IList)oObject;

                List<object> tmplist = new List<object>();
                foreach (object obj in list)
                {
                    tmplist.Add(obj);
                }
                Display(tmplist, fieldName);
            }
            else
            {
                throw new JceEncodeException("write object error: unsupport type.");
            }

            return this;
        }
        public JceDisplayer Display(JceStruct v, string fieldName)
        {
            Display('{', fieldName);
            if (null == v)
            {
                sb.Append('\t').Append("null");
            }
            else
            {
                v.Display(sb, _level + 1);
            }

            Display('}', null);
            return this;
        }
    }

}