
using System.Text;

using Wup.Jce;

namespace Wup.Jce//MSF.Engine.Wup
{
    internal enum JceStructType
    {
        BYTE = 0,
        SHORT = 1,
        INT = 2,
        LONG = 3,
        FLOAT = 4,
        DOUBLE = 5,
        STRING1 = 6,
        STRING4 = 7,
        MAP = 8,
        LIST = 9,
        STRUCT_BEGIN = 10,
        STRUCT_END = 11,
        ZERO_TAG = 12,
        SIMPLE_LIST = 13,
    }

    public abstract class JceStruct
    {
        public static int JCE_MAX_STRING_LENGTH = 100 * 1024 * 1024;

        public abstract void WriteTo(JceOutputStream _os);
        public abstract void ReadFrom(JceInputStream _is);
        public virtual void Display(StringBuilder sb, int level)
        {
        }
   }
}