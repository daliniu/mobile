
#define DEBUG

using System;
using System.Net;
//using System.Windows;
//using System.Windows.Controls;
//using System.Windows.Documents;
//using System.Windows.Ink;
//using System.Windows.Input;
//using System.Windows.Media;
//using System.Windows.Media.Animation;
//using System.Windows.Shapes;

using System.Collections;


namespace Wup
{
    public class QTrace
    {
        static public void Trace(string value)
        {
//#if DEBUG
            string strTrace = DateTime.Now.Hour.ToString("D2") + ":" + DateTime.Now.Minute.ToString("D2") + ":" + DateTime.Now.Second.ToString("D2") + ":" + DateTime.Now.Millisecond.ToString("D3");
            strTrace += "\t" + value;
            System.Diagnostics.Debug.WriteLine(strTrace);
//#endif
        }
        static public void Trace(string value, object arg)
        {
#if DEBUG
            string strTrace = string.Format("{0}{1}", value, arg);
            System.Diagnostics.Debug.WriteLine(value);
#endif
        }
        static public string Trace(byte[] value)
        {
            string strValue = "\r\n";
//#if DEBUG
            int i = 0;
            foreach (byte bt in value)
            {
                strValue += string.Format(" {0,02:x}", bt);
                i++;
                if ((i % 16) == 0)
                {
                    strValue += "\n";
                }
            }

            Trace(strValue);
//#endif
            return strValue;
        }

        static public void Trace(IDictionary dict)
        {
            if (dict == null)
            {
                return;
            }
            string strTrace = " Dictionary: ";
            foreach (object key in dict.Keys)
            {
                strTrace += key.ToString();
                strTrace += "\t\t";
                strTrace += dict[key].ToString();
                strTrace += "\r\n";
            }
            Trace(strTrace);
        }

        static public void Trace(IList list)
        {
            if (list == null)
            {
                return;
            }
            string strTrace = " List: ";
            foreach (object item in list)
            {
                strTrace += item.ToString();
                strTrace += "\r\n";
            }
            Trace(strTrace);
        }

        static public void Assert(bool condition)
        {
            System.Diagnostics.Debug.Assert(condition);
        }

        static public string Output(byte[] value)
        {
            string strValue = "";
#if DEBUG
            int i = 0;
            foreach (byte bt in value)
            {
                strValue += string.Format("0x{0:x},", bt);
                i++;
                if ((i % 16) == 0)
                {
                    strValue += "\n";
                }
            }
#endif
            return strValue;
        }
    }
}
