using System;
using System.Collections.Generic;

namespace Wup
{
    /**
     * 通过WUP调用TAF需要使用的包类型
     */
    internal class TafUniPacket : UniPacket
    {
        public TafUniPacket()
        {
            _package.iVersion = (short)2;
            _package.cPacketType = Const.PACKET_TYPE_WUP;
            _package.iMessageType = (int)0;
            _package.iTimeout = (int)0;
            _package.sBuffer = new byte[] { };
            _package.context = new Dictionary<string, string>();
            _package.status = new Dictionary<string, string>();
        }

        /**
         * 设置协议版本
         */
        public void setTafVersion(short version)
        {
            setVersion(version);
        }

        /**
         * 设置调用类型
         */
        public void setTafPacketType(byte packetType)
        {
            _package.cPacketType = packetType;
        }

        /**
         * 设置消息类型
         */
        public void setTafMessageType(int messageType)
        {
            _package.iMessageType = messageType;
        }

        /**
         * 设置超时时间
         */
        public void setTafTimeout(int timeout)
        {
            _package.iTimeout = timeout;
        }

        /**
         * 设置参数编码内容
         */
        public void setTafBuffer(byte[] buffer)
        {
            _package.sBuffer = buffer;
        }

        /**
         * 设置上下文
         */
        public void setTafContext(Dictionary<string, string> context)
        {
            _package.context = context;
        }

        /**
         * 设置特殊消息的状态值
         */
        public void setTafStatus(Dictionary<string, string> status)
        {
            _package.status = status;
        }

        /**
         * 获取协议版本
         */
        public short getTafVersion()
        {
            return _package.iVersion;
        }

        /**
         * 获取调用类型
         */
        public byte getTafPacketType()
        {
            return _package.cPacketType;
        }

        /**
         * 获取消息类型
         */
        public int getTafMessageType()
        {
            return _package.iMessageType;
        }

        /**
         * 获取超时时间
         */
        public int getTafTimeout()
        {
            return _package.iTimeout;
        }

        /**
         * 获取参数编码后内容
         */
        public byte[] getTafBuffer()
        {
            return _package.sBuffer;
        }

        /**
         * 获取上下文信息
         */
        public Dictionary<string, string> getTafContext()
        {
            return _package.context;
        }

        /**
         * 获取特殊消息的状态值
         */
        public Dictionary<string, string> getTafStatus()
        {
            return _package.status;
        }

        /**
         * 获取调用taf的返回值
         */
        public int getTafResultCode()
        {
            int result = 0;
            try
            {
                string rcode = _package.status[(Const.STATUS_RESULT_CODE)];
                result = (rcode != null ? int.Parse(rcode) : 0);
            }
            catch (Exception e)
            {
                QTrace.Trace(e.Message);
                return 0;
            }
            return result;
        }

        /**
         * 获取调用taf的返回描述
         */
        public string getTafResultDesc()
        {
            string rdesc = _package.status[(Const.STATUS_RESULT_DESC)];
            string result = rdesc != null ? rdesc : "";
            return result;
        }

    }
}