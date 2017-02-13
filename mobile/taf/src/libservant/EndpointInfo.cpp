#include "servant/EndpointInfo.h"
#include "log/taf_logger.h"
namespace taf
{
EndpointInfo::EndpointInfo()
: _port(0)
, _grid(0)
, _type(TCP)
{
    _qos=0;
    _setDivision.clear();
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	memset(_host, 0, sizeof(_host));
    //7暂时写死 修改默认值一定要修改这个7
    memcpy(_host,"0.0.0.0",7);
	//snprintf(_host, sizeof(_host), "%s", "0.0.0.0");
}

EndpointInfo::EndpointInfo(const string& host, uint16_t port, EndpointInfo::EType type, int32_t grid,const string & setDivision,int qos)
: _port(port)
, _grid(grid)
, _type(type)
{
    _qos=qos;
    _setDivision=setDivision;
	try
	{
		snprintf(_host, sizeof(_host), "%s", host.c_str());

		NetworkUtil::getAddress(_host, _port, _addr);
        _sCompareDesc = createCompareDesc();

		//_fulldesc = createFullDesc();

		//_desc = createDesc();

		//_keydesc = createKeyDesc(false);
	}
	catch (...)
	{
		LOG->error() << "[ERROR:getAddress fail:" << _host << ":" << _port << "]" << endl;
	}
}

#if 0
const string& EndpointInfo::descNoSetInfo() const
{
	return _descNoSetInfo;
}
#endif

//const string& EndpointInfo::desc() const
string EndpointInfo::desc() const
{
	//return _desc;
	return createDesc();
}

string EndpointInfo::createCompareDesc()
{
	ostringstream os;
	if (_type == EndpointInfo::UDP) { os << "udp:"; }
	if (_type == EndpointInfo::TCP) { os << "tcp:"; }
	os << _grid << ":" << _host << ":" << _port
        << ":" << _setDivision << ":" << _qos;

	return os.str();
}

string EndpointInfo::createDesc() const
{
	ostringstream os;
	os << (type() == EndpointInfo::TCP?"tcp":(type() == EndpointInfo::UDP?"udp":""));
	os << " -h " << host();
	os << " -p " << port();
    if(0 != _grid)
	    os << " -g " << _grid;

	if (!_setDivision.empty())
	{
		os << " -s " << _setDivision;
	}

    if(0 != _qos)
		os << " -q " << _qos;

	return os.str();
}

#if 0
EndpointInfo& EndpointInfo::operator = (const EndpointInfo& r)
{
    if (this != &r)
    {
		snprintf(_host, sizeof(_host), "%s", r._host);

        _addr = r._addr;
        _port = r._port;
        _type = r._type;
        _grid = r._grid;
		_setDivision = r._setDivision;
        _qos = r.qos;
    }
    return *this;
}
#endif

bool EndpointInfo::operator == (const EndpointInfo& r) const
{
    return (_sCompareDesc == r._sCompareDesc);
}

#if 0
bool EndpointInfo::equalNoSetInfo(const EndpointInfo& r) const
{
    return (this->descNoSetInfo() == r.descNoSetInfo());
}
#endif

bool EndpointInfo::operator < (const EndpointInfo& r) const
{
    //return (this->desc() < r.desc());
    return (_sCompareDesc < r._sCompareDesc);
}

string EndpointInfo::host() const
{
    return string(_host);
}

int32_t EndpointInfo::grid() const
{
    return _grid;
}

uint16_t EndpointInfo::port() const
{
    return _port;
}

const struct sockaddr_in& EndpointInfo::addr() const
{
    return _addr;
}

EndpointInfo::EType EndpointInfo::type() const
{
    return _type;
}

#if 0
void EndpointInfo::setDivision(const string & sSetDivision)
{
    _setDivision = sSetDivision;

    _sCompareDesc = createCompareDesc();

    //_fulldesc = createFullDesc();
}
#endif

const string& EndpointInfo::setDivision() const
{
    return _setDivision;
}
///////////////////////////////////////////////////////////
}
