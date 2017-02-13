using Wup.Jce;

using System;
using System.Text;
using System.Collections.Generic;
using System.IO;

namespace Wup
{
    public class UniPacket : UniAttribute
    {
        public static readonly int UniPacketHeadSize = 4;

        protected RequestPacket _package = new RequestPacket();

        /**
         * 获取请求的service名字
         * 
         * @return
         */
        public string ServantName
        {
            get
            {
                return _package.sServantName;
            }
            set
            {
                _package.sServantName = value;
            }
        }

        /**
         * 获取请求的函数名字
         * 
         * @return
         */
        public string FuncName
        {
            get
            {
                return _package.sFuncName;
            }
            set
            {
                _package.sFuncName = value;
            }
        }

        /**
         * 获取消息序列号
         * 
         * @return
         */
        public int RequestId
        {
            get
            {
                return _package.iRequestId;
            }
            set
            {
                _package.iRequestId = value;
            }
        }
        public UniPacket()
        {
            _package.iVersion = 2;
        }
        public void setVersion(short iVer)
        {
            _iVer = iVer;
            _package.iVersion = iVer;
        }
        public short getVersion()
        {
            return _package.iVersion;
        }
        /**
         * 将put的对象进行编码
         */
        public new byte[] Encode()
        {
            if (_package.sServantName.Equals(""))
            {
                throw new ArgumentException("servantName can not is null");
            }
            if (_package.sFuncName.Equals(""))
            {
                throw new ArgumentException("funcName can not is null");
            }

            JceOutputStream _os = new JceOutputStream(0);
            _os.setServerEncoding(EncodeName);
            if (_package.iVersion == Const.PACKET_TYPE_WUP)
            {
                _os.Write(_data, 0);
            }
            else
            {
                _os.write(_new_data, 0);
            }

            _package.sBuffer = JceUtil.getJceBufArray(_os.getMemoryStream());
            
            _os = new JceOutputStream(0);
            _os.setServerEncoding(EncodeName);
            this.WriteTo(_os);
            byte[] bodys = JceUtil.getJceBufArray(_os.getMemoryStream());
            int size = bodys.Length;

            MemoryStream ms = new MemoryStream(size + UniPacketHeadSize);

            using (BinaryWriter bw = new BinaryWriter(ms))
            {
                // 整个数据包长度
                bw.Write(ByteConverter.ReverseEndian(size + UniPacketHeadSize));
                bw.Write(bodys);
            }

            return ms.ToArray();
        }

        /**
         * 对传入的数据进行解码 填充可get的对象
         */

        public new void Decode(byte[] buffer, int Index = 0)
        {
            if (buffer.Length < UniPacketHeadSize)
            {
                throw new ArgumentException("Decode namespace must include size head");
            }

            try
            {
                JceInputStream _is = new JceInputStream(buffer, UniPacketHeadSize + Index);
                _is.setServerEncoding(EncodeName);
                //解码出RequestPacket包
                this.ReadFrom(_is);

                //设置wup版本
                _iVer = _package.iVersion;

                _is = new JceInputStream(_package.sBuffer);
                _is.setServerEncoding(EncodeName);

                if (_package.iVersion == Const.PACKET_TYPE_WUP)
                {
                    _data = (Dictionary<string, Dictionary<string, byte[]>>)_is.readMap<Dictionary<string, Dictionary<string, byte[]>>>(0, false);
                }
                else
                {
                    _new_data = (Dictionary<string, byte[]>)_is.readMap<Dictionary<string, byte[]>>(0, false);  
                }
            }
            catch (Exception e)
            {
                QTrace.Trace(this + " Decode Exception: " + e.Message);
                throw (e);
            }
        }

        public override void WriteTo(JceOutputStream _os)
        {
            _package.WriteTo(_os);
        }

        public override void ReadFrom(JceInputStream _is)
        {
            _package.ReadFrom(_is);
        }
        
        public int OldRespIRet { get; set; }

        /**
         * 为兼容最早发布的客户端版本解码使用 iret字段始终为0
         * 
         * @return
         */
        public byte[] CreateOldRespEncode()
        {
            JceOutputStream _os = new JceOutputStream(0);
            _os.setServerEncoding(EncodeName);
            _os.Write(_data, 0);
            byte[] oldSBuffer = JceUtil.getJceBufArray(_os.getMemoryStream());
            _os = new JceOutputStream(0);
            _os.setServerEncoding(EncodeName);
            _os.Write(_package.iVersion, 1);
            _os.Write(_package.cPacketType, 2);
            _os.Write(_package.iRequestId, 3);
            _os.Write(_package.iMessageType, 4);
            _os.Write(OldRespIRet, 5);
            _os.Write(oldSBuffer, 6);
            _os.Write(_package.status, 7);
            return JceUtil.getJceBufArray(_os.getMemoryStream());
        }
    }
}