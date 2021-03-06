// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 1.5.1 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/MiniGameProto_new.jce'
// **********************************************************************

package MiniGameProto;

public final class TMsgResponseListRoom extends com.qq.taf.jce.JceStruct
{
    public String className()
    {
        return "MiniGameProto.TMsgResponseListRoom";
    }

    public short nResultID = (short)0;

    public short nGameID = (short)0;

    public short nTotalNum = (short)0;

    public java.util.Vector vecRoomList = new java.util.Vector();

    public short getNResultID()
    {
        return nResultID;
    }

    public void  setNResultID(short nResultID)
    {
        this.nResultID = nResultID;
    }

    public short getNGameID()
    {
        return nGameID;
    }

    public void  setNGameID(short nGameID)
    {
        this.nGameID = nGameID;
    }

    public short getNTotalNum()
    {
        return nTotalNum;
    }

    public void  setNTotalNum(short nTotalNum)
    {
        this.nTotalNum = nTotalNum;
    }

    public java.util.Vector getVecRoomList()
    {
        return vecRoomList;
    }

    public void  setVecRoomList(java.util.Vector vecRoomList)
    {
        this.vecRoomList = vecRoomList;
    }

    public TMsgResponseListRoom()
    {
    }

    public TMsgResponseListRoom(short nResultID, short nGameID, short nTotalNum, java.util.Vector vecRoomList)
    {
        this.nResultID = nResultID;
        this.nGameID = nGameID;
        this.nTotalNum = nTotalNum;
        this.vecRoomList = vecRoomList;
    }

    public boolean equals(Object o)
    {
        TMsgResponseListRoom t = (TMsgResponseListRoom) o;
        return (
            com.qq.taf.jce.JceUtil.equals(nResultID, t.nResultID) && 
            com.qq.taf.jce.JceUtil.equals(nGameID, t.nGameID) && 
            com.qq.taf.jce.JceUtil.equals(nTotalNum, t.nTotalNum) && 
            com.qq.taf.jce.JceUtil.equals(vecRoomList, t.vecRoomList) );
    }

    public void writeTo(com.qq.taf.jce.JceOutputStream _os)
    {
        try
        {
            _os.write(nResultID, 0);
            _os.write(nGameID, 1);
            _os.write(nTotalNum, 2);
            _os.write(vecRoomList, 3);
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public void readFrom(com.qq.taf.jce.JceInputStream _is)
    {
        try
        {
            nResultID = 0;
            nResultID = (short) _is.read(nResultID, 0, true);

            nGameID = 0;
            nGameID = (short) _is.read(nGameID, 1, true);

            nTotalNum = 0;
            nTotalNum = (short) _is.read(nTotalNum, 2, true);

            vecRoomList = new java.util.Vector();
            MiniGameProto.TRoomInfo __var_13 = new MiniGameProto.TRoomInfo();
            vecRoomList.addElement(__var_13);
            vecRoomList = (java.util.Vector) _is.read(vecRoomList, 3, true);

        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public void display(java.lang.StringBuffer _os, int _level)
    {
    }

}
