#ifndef _JMEM_POLICY_H
#define _JMEM_POLICY_H

#include <fcntl.h>
#include <algorithm>
#include "util/tc_file.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_sem_mutex.h"
#include "util/tc_shm.h"
#include "util/tc_mmap.h"

namespace taf
{
//////////////////////////////////////////////////////////////////////
// 存储策略: 内存, 共享内存, mmap(文件)

/**
 * 内存存储
 */
template<typename T, typename LockPolicy>
class MemStorePolicy : public LockPolicy
{
public:
    /**
    * 初始化
    * @param pAddr: 指令队列空间的指针
    * @param iSize: 空间的指针
    */
    void create(void *pAddr, size_t iSize)
    {
        _t.create(pAddr,iSize);
    }

    /**
    * 连接上队列
    * @param pAddr: 指令队列空间的指针
    * @param iSize: 空间的指针
    */
    void connect(void *pAddr, size_t iSize)
    {
        _t.connect(pAddr,iSize);
    }

protected:
    T   _t;
};

/**
 * 共享内存存储
 */
template<typename T, typename LockPolicy>
class ShmStorePolicy : public LockPolicy
{
public:
    /**
     * 初始化共享存储
     * @param iShmKey
     * @param iSize
     */
    void initStore(key_t iShmKey, size_t iSize)
    {
        _shm.init(iSize, iShmKey);
        if(_shm.iscreate())
        {
            _t.create(_shm.getPointer(), iSize);
        }
        else
        {
            _t.connect(_shm.getPointer(), iSize);
        }
    }

    /**
     * 释放共享内存
     */
    void release()
    {
        _shm.del();
    }
protected:
    TC_Shm  _shm;
    T       _t;
};

template<typename T, typename LockPolicy>
class ShmFileStorePolicy : public LockPolicy
{
    class ShmStore
    {
    public:
        int shmopen( const char* file, size_t size )
        {
            int fd = -1;
            if ( ( fd = shm_open( file, O_RDWR | O_CREAT | O_EXCL, 0644 ) ) >= 0 )
            {
                ftruncate( fd, size );
                _bCreate = true;
            }
            else if ( errno == EEXIST )
            {
                if ( ( fd = shm_open( file, O_RDWR | O_CREAT, 0644 ) ) >= 0 )
                {
                    ftruncate( fd, size );
                    _bCreate = false;
                }
                else
                {
                    throw TC_Mmap_Exception( "[TC_Mmap::mmap] fopen shmfile '" + string( file ) + "' error", errno );
                }
            }
            else
            {
                throw TC_Mmap_Exception( "[TC_Mmap::mmap] fopen shmfile '" + string( file ) + "' error", errno );
            }
            return fd;
        }
        void shmfree( int fd )
        {
            close( fd );
        }
        void shmclose( const char* file )
        {
            shm_unlink( file );
        }

        /* 
         * @brief 是否创建出来的，文件的存在可分为两种形式：
         * 
         * 一种是创建的，一种是开始就存在的
         * @return 如果是创建出来的就返回true，否则返回false
         */
        bool iscreate() const
        {
            return _bCreate;
        }

    protected:
        /*
         * 是否创建出来的
         */
        bool _bCreate;
    };

public:
    /**
     * 初始化共享存储
     * @param iShmKey
     * @param iSize
     */
    void initStore( const char* file, size_t iSize )
    {
        _file = file;
        std::replace(_file.begin(),_file.end(),'/','_');
        int fd = -1;
        string filename = string( "/dev/shm/" ) + _file;
        if ( TC_File::isFileExist( filename ) )
        {
            iSize = TC_File::getFileSize( filename );
        }
        if ( ( fd = _ss.shmopen( _file.c_str(), iSize ) ) >= 0 )
        {
            _mmap.mmap( iSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

            if ( _ss.iscreate() )
            {
                _t.create( _mmap.getPointer(), _mmap.getSize() );
            }
            else
            {
                _t.connect( _mmap.getPointer(), _mmap.getSize() );
            }
            _ss.shmfree( fd );
        }
    }

    /**
     * 扩展空间, 目前只对hashmap有效
     */
    int expand( size_t iSize )
    {
        TC_LockT<typename LockPolicy::Mutex> lock( LockPolicy::mutex() );
        if ( iSize < _mmap.getSize() )
        {
            return -1;
        }
        int fd = -1;
        int ret = -1;
        if ( ( fd = _ss.shmopen( _file.c_str(), iSize ) ) >= 0 )
        {
            TC_Mmap m(false);
            m.mmap( iSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

            if((ret=_t.append( m.getPointer(), m.getSize()))==0)
            {
                _mmap.munmap();
                _mmap=m;
                _mmap.setOwner(true);
            }
            else
            {
                m.munmap();
            }
            _ss.shmfree(fd);
        }
        return ret;
    }

    /**
     * 释放共享内存
     */
    void release()
    {
        _ss.shmclose(_file.c_str());
    }
protected:
    string  _file;
    ShmStore _ss;
    TC_Mmap _mmap;
    T     _t;
};

/**
 * 文件存储
 */
template<typename T, typename LockPolicy>
class FileStorePolicy : public LockPolicy
{
public:
    /**
     * 初始化文件
     * @param file, 文件路径
     * @param iSize, 文件大小
     */
    void initStore(const char *file, size_t iSize)
    {
        _file = file;
        if ( TC_File::isFileExist( _file) )
        {
            iSize = TC_File::getFileSize( _file);
        }
        _mmap.mmap(file, iSize);
        if(_mmap.iscreate())
        {
            _t.create(_mmap.getPointer(), iSize);
        }
        else
        {
            _t.connect(_mmap.getPointer(), iSize);
        }
    }

    /**
     * 扩展空间, 目前只对hashmap有效
     */
    int expand(size_t iSize)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        TC_Mmap m(false);
        m.mmap(_file.c_str(), iSize);

        int ret = _t.append(m.getPointer(), iSize);

        if(ret == 0)
        {
            _mmap.munmap();
            _mmap = m;
            _mmap.setOwner(true);
        }
        else
        {
            m.munmap();
        }

        return ret;
    }

protected:
    string  _file;
    TC_Mmap _mmap;
    T       _t;
};

//////////////////////////////////////////////////////////////////////
// 锁策略: 无锁, 线程锁, 进程锁

/**
 * 无锁
 */
class EmptyLockPolicy
{
public:
    typedef TC_EmptyMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * 线程锁策略
 */
class ThreadLockPolicy
{
public:
    typedef TC_ThreadMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * 进程锁策略
 */
class SemLockPolicy
{
public:
    typedef TC_SemMutex Mutex;
    void initLock(key_t iSemKey)    { return _mutex.init(iSemKey); }
    Mutex &mutex()                  { return _mutex; }

protected:
    Mutex _mutex;
};

}

#endif
