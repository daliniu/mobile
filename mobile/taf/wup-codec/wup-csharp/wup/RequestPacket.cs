using Wup;
using System.Collections.Generic;
using System;
using System.Text;
using Wup.Jce;

namespace Wup
{
    public class RequestPacket : Wup.Jce.JceStruct
    {
        public short iVersion = 0;

        public byte cPacketType = 0;

        public int iMessageType = 0;

        public int iRequestId = 0;

        public string sServantName = null;

        public string sFuncName = null;

        public byte[] sBuffer;

        public int iTimeout = 0;

        public Dictionary<string, string> context;

        public Dictionary<string, string> status;

        public RequestPacket()
        {
        }

        public RequestPacket(short iVersion, byte cPacketType, int iMessageType, int iRequestId, string sServantName, string sFuncName, byte[] sBuffer, int iTimeout, Dictionary<string, string> context, Dictionary<string, string> status)
        {
            this.iVersion = iVersion;
            this.cPacketType = cPacketType;
            this.iMessageType = iMessageType;
            this.iRequestId = iRequestId;
            this.sServantName = sServantName;
            this.sFuncName = sFuncName;
            this.sBuffer = sBuffer;
            this.iTimeout = iTimeout;
            this.context = context;
            this.status = status;
        }

        public override void WriteTo(Wup.Jce.JceOutputStream _os)
        {
            _os.Write(iVersion, 1);
            _os.Write(cPacketType, 2);
            _os.Write(iMessageType, 3);
            _os.Write(iRequestId, 4);
            _os.Write(sServantName, 5);
            _os.Write(sFuncName, 6);
            _os.Write(sBuffer, 7);
            _os.Write(iTimeout, 8);
            _os.Write(context, 9);
            _os.Write(status, 10);
            //System.out.println(sFuncName+" WriteTo STATUS_SAMPLE_KEY:"+status.Get("STATUS_SAMPLE_KEY"));
        }

        static byte[] cache_sBuffer = null;

        public override void ReadFrom(Wup.Jce.JceInputStream _is)
        {
            try
            {
                iVersion = (short)_is.Read(iVersion, 1, true);
                cPacketType = (byte)_is.Read(cPacketType, 2, true);
                iMessageType = (int)_is.Read(iMessageType, 3, true);
                iRequestId = (int)_is.Read(iRequestId, 4, true);
                sServantName = (string)_is.readString(5, true);
                sFuncName = (string)_is.readString(6, true);
                
                if (null == cache_sBuffer)
                {
                    cache_sBuffer = new byte[] { 0 };
                }
                sBuffer = (byte[])_is.Read<byte[]>(cache_sBuffer, 7, true);
                iTimeout = (int)_is.Read(iTimeout, 8, true);
                
                Dictionary<string, string> cache_context = null;
                context = (Dictionary<string, string>)_is.Read<Dictionary<string, string>>(cache_context, 9, true);
                status = (Dictionary<string, string>)_is.Read<Dictionary<string, string>>(cache_context, 10, true);
            }
            catch (Exception e)
            {
                QTrace.Trace(this + " ReadFrom Exception: " + e.Message);
                throw e;
            }


        }
        
    }
}