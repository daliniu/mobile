#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "util/tc_fifo.h"

namespace taf
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// 
TC_Fifo::TC_Fifo(bool bOwner) : _bOwner(bOwner), _enRW(EM_READ), _fd(-1)
{

}

TC_Fifo::~TC_Fifo()
{
	if (_bOwner) close();
}

void TC_Fifo::close()
{
	if (_fd >= 0) ::close(_fd); 
		
	_fd = -1;
}

int TC_Fifo::open(const std::string & sPathName, ENUM_RW_SET enRW, mode_t mode)
{
	_enRW       = enRW;
	_sPathName	= sPathName;

	if (_enRW != EM_READ && _enRW != EM_WRITE)
	{
		return -1;
	}

	if (::mkfifo(_sPathName.c_str(), mode) == -1 && errno != EEXIST)
	{
		return -1;
	}

	if (_enRW == EM_READ  && (_fd = ::open(_sPathName.c_str(), O_NONBLOCK|O_RDONLY, 0664)) < 0)
	{
		return -1;
	}

	if (_enRW == EM_WRITE && (_fd = ::open(_sPathName.c_str(), O_NONBLOCK|O_WRONLY, 0664)) < 0)
	{
		return -1;
	}

	return 0;
}

int TC_Fifo::read(char * szBuff, const size_t sizeMax)
{
	return ::read(_fd, szBuff, sizeMax);
}

int TC_Fifo::write(const char * szBuff, const size_t sizeBuffLen)
{
	if (sizeBuffLen == 0) return 0;

	return ::write(_fd, szBuff, sizeBuffLen);
}

}


