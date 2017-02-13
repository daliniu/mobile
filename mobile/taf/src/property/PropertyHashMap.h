#ifndef __PROPERTY_HASHMAP_H_
#define __PROPERTY_HASHMAP_H_

#include "util/tc_common.h"
#include "jmem/jmem_hashmap.h"
#include "servant/PropertyF.h"
using namespace taf;

//extern TC_Config * g_pconf;
//class PropHead;
typedef StatPropMsgBody PropBody;
typedef StatPropMsgHead PropHead;
typedef JceHashMap<PropHead,PropBody, ThreadLockPolicy, FileStorePolicy> PropHashMap;

//typedef std::map<taf::StatPropMsgHead, taf::StatPropMsgBody, std::less<taf::StatPropMsgHead>, __gnu_cxx::__pool_alloc<std::pair<taf::StatPropMsgHead const, taf::StatPropMsgBody> > > StatMsg;
#if 0
struct PropHead : public taf::JceStructBase
{
    PropHead()
    :  /*time(0),requestId(0),*/moduleName(""),ip(""),propertyName(""),setName(""),setArea(""),setID("")
    {
    }
    template < typename WriterT >
    void writeTo(taf::JceOutputStream<WriterT>& _os) const
    {
    /*
        _os.write(time, 0);
        _os.write(requestId, 1);
        */
        _os.write(moduleName, 2);
        _os.write(ip, 3);
        _os.write(propertyName, 4);
        _os.write(setName, 5);
        _os.write(setArea, 6);
        _os.write(setID, 7);
    }
    template < typename ReaderT >
    void readFrom(taf::JceInputStream<ReaderT>& _is)
    {   
    /*
        _is.read(time, 0, true);
        _is.read(requestId, 1, true);
        */
        _is.read(moduleName, 2, true);
        _is.read(ip, 3, true);
        _is.read(propertyName, 4, true);
        _is.read(setName, 5, false);
        _is.read(setArea, 6, false);
        _is.read(setID, 7, false);
    }

	inline bool operator<(const PropHead &h) const
    {
        if(moduleName != h.moduleName)  return (moduleName < h.moduleName);
        if(ip != h.ip)  return (ip < h.ip);
        if(propertyName != h.propertyName) return (propertyName < h.propertyName);
		if(setName != h.setName) return (setName < h.setName);
		if(setArea != h.setArea) return (setArea < h.setArea);
		return (setID < h.setID);
    }
    /*
    bool operator<(const PropHead &h)const
    {
        if(time != h.time) return (time < h.time);
        return (requestId < h.requestId);
    }

*/


	
public:
    //int         time;
    //unsigned    requestId;
    string      moduleName;
    string      ip;
    string      propertyName;   
	string      setName;  
	string      setArea;
	string      setID;
};
#endif



class PropertyHashMap : public PropHashMap
{
public:


    /**
    * 增加数据
    * @param Key
    * @param Value
    *
    * @return int
    */
    int add(const PropHead &head, const StatPropMsgBody &body)
    {
    
 		ostringstream os;
		os.str("");
        PropBody stBody;
        int ret = TC_HashMap::RT_OK;
        taf::JceOutputStream<BufferWriter> osk;
        head.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
        string sv;
        time_t t = 0;
        ret = this->_t.get(sk, sv,t);
      
        if ( ret < 0 || ret == TC_HashMap::RT_ONLY_KEY || ret == TC_HashMap::RT_NO_DATA)
        {

			taf::JceOutputStream<BufferWriter> osv;
			body.writeTo(osv);
			string stemp(osv.getBuffer(), osv.getLength());
			vector<TC_HashMap::BlockData> vtData;
			return	this->_t.set(sk, stemp,true,vtData);

		}
        //读取到数据了, 解包
        if (ret == TC_HashMap::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            stBody.readFrom(is);
			stBody.displaySimple(os);
			LOG->info()<< "read hash body|" <<os.str()<< endl;
			os.str("");
        }
		

		size_t len =0;
		long long  count=0;
		long long  sum=0;
		double avg=0;
		long long  min=0;
		long long  max=0;
		len = body.vInfo.size();
		string sPolicy = "";
		string sValue = "";
		string inValue = "";
		
		for (size_t i=0; i< len; i++)
		{
			sPolicy = stBody.vInfo[i].policy;
			sValue = stBody.vInfo[i].value;
			inValue = body.vInfo[i].value;
			vector<string> fields;
			vector<string> fieldIn;
			
			if (sPolicy == "Count" )
			{
				count = TC_Common::strto<long long >(sValue) + TC_Common::strto<long long >(inValue);
				stBody.vInfo[i].value = TC_Common::tostr(count);
			}
			else if (sPolicy == "Sum")
			{
				sum =  TC_Common::strto<long long >(sValue) + TC_Common::strto<long long >(inValue);
				stBody.vInfo[i].value = TC_Common::tostr(sum);

			}

			else if (sPolicy == "Min")
			{
				long long  s= TC_Common::strto<long long >(sValue);
				long long  in= TC_Common::strto<long long >(inValue);
        		min =  s < in?s:in;
				stBody.vInfo[i].value = TC_Common::tostr(min);
			}			
			else if (sPolicy == "Max")
			{
        		long long  s= TC_Common::strto<long long >(sValue);
				long long  in= TC_Common::strto<long long >(inValue);
        		max =  s > in?s:in;
				stBody.vInfo[i].value = TC_Common::tostr(max);
			}
			else if (sPolicy == "Distr")
			{
				fields = TC_Common::sepstr<string>(sValue, ",");
				fieldIn= TC_Common::sepstr<string>(inValue, ",");
				string tmpValue = "";
				for (size_t k=0; k<fields.size(); k++)
				{
					vector<string> sTmp = TC_Common::sepstr<string>(fields[k], "|");
					vector<string> inTmp = TC_Common::sepstr<string>(fieldIn[k], "|");
					long long  tmp = TC_Common::strto<long long >(sTmp[1]) + TC_Common::strto<long long >(inTmp[1]);
					sTmp[1] = TC_Common::tostr(tmp);
					fields[k] = sTmp[0]+ "|" +sTmp[1];

					if (k==0)
					{
						tmpValue = fields[k];
					}
					else
					{
						tmpValue  = tmpValue + "," + fields[k];
					}
				}
				stBody.vInfo[i].value = tmpValue;
			}

			if (sPolicy == "Avg")
			{
				if (count !=0 && sum != 0)
				{
					avg = (double)sum /count;
				}
				else
				{
					avg = (TC_Common::strto<double>(sValue) + TC_Common::strto<double>(inValue))/2;
				}
				stBody.vInfo[i].value = TC_Common::tostr(avg);
			}

		}
		stBody.displaySimple(os);
        LOG->info()<<"reset hash body|" <<os.str()<< endl;

        taf::JceOutputStream<BufferWriter> osv;
        stBody.writeTo(osv);
        string stemp(osv.getBuffer(), osv.getLength());
        vector<TC_HashMap::BlockData> vtData;
        return  this->_t.set(sk, stemp,true,vtData);
    }
};


extern PropertyHashMap g_hashmap;






#endif


