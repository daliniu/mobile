#ifndef __QUANBASE_H_
#define __QUANBASE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace QUAN
{
	const long gUIdXorNum = 5253;
	const long gUserVersion = 101;

#define SECRET_ID_BEGIN -1
#define SECRET_ID_END   -1

#define LOL_GOD 7500

#define PROC_BEGIN do{
#define PROC_END   }while(0);


#define __TRY__ try\
	{

#define __CATCH__ }\
	catch (std::exception const& e)\
	{\
	LOG->error() << __FILE__ << "::" << __FUNCTION__ << string(" catch std exception: ") + e.what() << endl; \
	}\
	catch (...)\
	{\
	LOG->error() << __FILE__ << "::" << __FUNCTION__ << " catch unknown exception" << endl; \
	}

	template <typename T> string JCETOSTR(T &t)
	{
		ostringstream os;
		t.displaySimple(os);
		return os.str();
	}

	template<typename T>  string JCETOSTR(const vector<T> &t)
	{
		ostringstream os;
		for (unsigned i = 0; i < t.size(); i++)
		{
			os << JCETOSTR(t[i]);
			os << "|";
		}
		return os.str();
	}

	template<typename T>  string JCETOSTR(vector<map<T, Int32> > const &t)
	{
		ostringstream os;
		/*
		for(unsigned i=0;i < t.size();i++)
		{
		for(unsigned j=0;j < t[i].size();j++)
		{
		os<<t[i][j];
		os<<"|";
		}
		}
		*/
		return os.str();
	}

	template<typename T>  string JCETOSTR(vector<T> &t)
	{
		ostringstream os;
		for (unsigned i = 0; i < t.size(); i++)
		{
			os << JCETOSTR(t[i]);
			os << "|";
		}
		return os.str();
	}

	template <typename T> string SIMPLEJCETOSTR(T &t)
	{
		ostringstream os;
		t.displaySimple(os);
		return os.str();
	}

	template<typename T>  string SIMPLEJCETOSTR(const vector<T> &t)
	{
		ostringstream os;
		for (unsigned i = 0; i < t.size(); i++)
		{
			os << JCETOSTR(t[i]);
			os << "|";
		}
		return os.str();
	}

	template<typename T>  string SIMPLEJCETOSTR(vector<T> &t)
	{
		ostringstream os;
		for (unsigned i = 0; i < t.size(); i++)
		{
			os << JCETOSTR(t[i]);
			os << "|";
		}
		return os.str();
	}

	template<typename T1, typename T2>  string SIMPLEJCETOSTR(map<T1, T2> const& t)
	{
		ostringstream os;
		for (typename map<T1, T2>::const_iterator i = t.begin(); i != t.end(); ++i)
		{
			os << " first: | ";
			os << i->first;
			os << " second :| FirstId: ";
			os << i->second.lFirstId;
			os << " second :| RELATION: ";
			os << i->second.iRelat;
			os << " second :| ISREAD: ";
			os << i->second.isRead;
			os << " | " << endl;
		}
		return os.str();
	}

	template<typename T>  string simpledisplay(vector<T> const& t)
	{
		ostringstream os;
		for (unsigned i = 0; i < t.size(); i++)
		{
			os << TC_Common::tostr(t[i]);
			os << "|";
		}
		return os.str();
	}

	template<typename T1, typename T2>
	void display(string const& name, map<T1, T2> const& container)
	{
		int j = 0;
		ostringstream os;
		os << name;
		for (typename map<T1, T2>::const_iterator i = container.begin(); i != container.end(); ++i)
		{
			ostringstream os1; i->second.displaySimple(os1);
			os << " |" << j << " first: |" << i->first << " second: |" << os1.str() << "|";
			j++;
		}
		LOG->info() << os.str() << endl;
	}

	template<typename T1>
	void display(string const& name, map<T1, bool> const& container)
	{
		int j = 0;
		ostringstream os;
		os << name;
		for (typename map<T1, bool>::const_iterator i = container.begin(); i != container.end(); ++i)
		{
			os << " |" << j << " first: " << i->first << " second: " << i->second << "|";
			j++;
		}
		LOG->info() << os.str() << endl;
	}

	template<typename T>
	void display(string const& name, T const& container)
	{
		int j = 0;
		ostringstream os;
		os << name;
		for (typename T::const_iterator i = container.begin(); i != container.end(); ++i)
		{
			ostringstream os1; i->displaySimple(os1);
			os << " |" << j << " : " << os1.str() << "|";
			j++;
		}
		LOG->info() << os.str() << endl;
	}


	inline void display(string const& name, vector<long> const& container)
	{
		int j = 0;
		ostringstream os;
		os << name << " size is :" << container.size();
		for (vector<long>::const_iterator i = container.begin(); i != container.end(); ++i)
		{
			os << " |" << j << " : " << *i << "|";
			j++;
		}
		LOG->info() << os.str() << endl;
	}


	template<typename T>
	void encodeResponse(T& container)
	{
		ostringstream os;
		os << " UId encode begin : ";
		for (typename T::iterator i = container.begin(); i != container.end(); ++i)
		{
			os << "|" << i->lUId << "|";
			i->lUId = encodeUId(i->lUId);
			os << "|" << i->lUId << "|";
		}
		os << " : UId encode end " << endl;

		LOG->info() << os.str() << endl;
	}

	inline long getId(long lUId, string& sId)
	{
		time_t now = TC_TimeProvider::getInstance()->getNow();

		Int64 lTmp = now << 32;
		Int64 lSId = lTmp + ((lUId & 0x00000000FFFFFFFF) ^ gUIdXorNum);

		stringstream ss;
		ss << lSId;
		sId = ss.str();

		return lSId;
	}

	inline long decodeSecretId(long lUId)
	{
		return (lUId & 0xFFFFFFFF00000000) >> 32;
	}

	inline long decodeSecretIdUId(long lUId)
	{
		return (lUId & 0x00000000FFFFFFFF) ^ gUIdXorNum;
	}

	inline long encodeUId(long lId)
	{
		srand(time(NULL));
		long lVersion = (lId & 0xFFFF000000000000) >> 48;
		ostringstream os;
		os << __FUNCTION__ << " Before Encode: " << lId;
		if (gUserVersion == lVersion)
		{
			os << " After Encode: " << lId;
			LOG->debug() << os.str() << endl;
			return lId;
		}
		else
		{
			long lE = ((gUserVersion & 0xFFFF) << 48) + (((lId & 0x00000000FFFFFFFF) ^ gUIdXorNum) << 16) + ((rand() % SHRT_MAX) & 0xFFFF);
			os << " After Encode: " << lE;
			LOG->debug() << os.str() << endl;
			return lE;
		}
	}

	inline int decodeUId(long lId)
	{
		long lVersion = (lId & 0xFFFF000000000000) >> 48;
		ostringstream os;
		os << __FUNCTION__ << " Before Decode: " << lId;
		if (gUserVersion == lVersion)
		{
			long lD = ((lId & 0x0000FFFFFFFF0000) >> 16) ^ gUIdXorNum;
			os << " After Decode: " << lD;
			LOG->debug() << os.str() << endl;
			return lD;
		}
		else
		{
			os << " After Decode: " << lId;
			LOG->debug() << os.str() << endl;
			return lId;
		}
	}

	template<typename T>
	inline map<Int64, T> FrdsVector2Map(vector<T>& v)
	{
		map<Int64, T> tTmp;
		for (typename vector<T>::iterator i = v.begin(); i != v.end(); ++i)
		{
			if (i->iRelat > 0)
				i->iRelat--;
			tTmp.insert(make_pair(i->lUId, *i));
		}
		return tTmp;
	}

	template<typename T>
	inline map<Int64, T> Vector2Map(vector<T> const& v)
	{
		map<Int64, T> tTmp;
		for (typename vector<T>::const_iterator i = v.begin(); i != v.end(); ++i)
		{
			tTmp.insert(make_pair(i->lUId, *i));
		}
		return tTmp;
	}

	template<typename T>
	inline map<Int64, T> Vector2Map(pair< vector<T>, vector<T> > const& v)
	{
		map<Int64, T> tTmp;
		for (typename vector<T>::const_iterator i = v.first.begin(); i != v.first.end(); ++i)
		{
			tTmp.insert(make_pair(i->lUId, *i));
		}
		for (typename vector<T>::const_iterator i = v.second.begin(); i != v.second.end(); ++i)
		{
			tTmp.insert(make_pair(i->lUId, *i));
		}
		return tTmp;
	}

	inline void split(string const& str, string const& delimiter, vector<string>& dest)
	{
		string::size_type pos = 0, found = 0;

		found = str.find(delimiter, pos);
		if (found == string::npos)
		{
			dest.push_back(str);
			return;
		}

		found = 0;
		while (found != string::npos)
		{
			found = str.find(delimiter, pos);
			if (found == string::npos)
			{
				Int32 e = str.size();
				// LOG->debug() << str << " " << pos << " " << e << endl;
				dest.push_back(string(str, pos, e - pos));
				break;
			}
			dest.push_back(string(str, pos, found - pos));
			pos = found + 1;
		}
	}
}
#endif
