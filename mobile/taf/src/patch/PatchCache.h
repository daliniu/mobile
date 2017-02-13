#ifndef __PATCH_CACHE_H__
#define __PATCH_CACHE_H__
#include <string>
#include <map>
#include <vector>
#include "util/tc_monitor.h"

const size_t SIZE_BUMEM_MIN =   5 * 1024 * 1024;
const size_t SIZE_BUMEM_MAX = 100 * 1024 * 1024;

class PatchCache
{
private:
    enum FileLoad 
    {
        EM_NLOAD    = 0,
        EM_LOADING  = 1,
        EM_RELOAD   = 2,
        EM_LOADED   = 3   
    };

    struct MemState
    {
        string      FileName;       //文件路径
        size_t      FileSize;       //文件大小
        size_t      FileInode;      //Inode索引号
        time_t      FileTime;       //文件创建时间

        char *      MemBuf;         //该块内存的起始地址
        size_t      MemSize;        //该块内存的大小
        size_t      MemCount;       //指向该块内存的数目
        time_t      MemTime;        //该块内存最后操纵时间
        FileLoad    MemLoad;        //该块内存已经加载成功
    };
public:
    void setMemOption(const size_t MemMax, const size_t MemMin, const size_t MemNum)
    {
        _MemMax     = MemMax > SIZE_BUMEM_MAX?SIZE_BUMEM_MAX:MemMax;
        _MemMin     = MemMin < SIZE_BUMEM_MIN?SIZE_BUMEM_MIN:MemMin;
        _MemNum     = MemNum;
    }

    int load(const std::string & sFile, std::pair<char *, size_t> & mem);

    int release(const std::string & sFile);

private:
    int __loadFile(const std::string & sFile, char * szBuff, size_t sizeLen);

    int __getMem(const std::string & sFile, struct MemState *& cur);

private:
    taf::TC_ThreadLock _mutex;

    std::vector<MemState *>             _vecMems;

    std::map<std::string, MemState *>   _mapFiles;

    size_t _MemMax;

    size_t _MemMin;

    size_t _MemNum;
};

#endif

