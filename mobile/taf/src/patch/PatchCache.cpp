#include <sys/stat.h>
#include <unistd.h>
#include "PatchCache.h"
#include "log/taf_logger.h"

using namespace taf;

int PatchCache::load(const std::string & sFile, std::pair<char *, size_t> & mem)
{
    LOG->debug() << __FUNCTION__ << "|" << sFile << endl;
    struct MemState * cur = NULL;

    try
    {
        struct stat st;
        memset(&st, 0, sizeof(struct stat));
        if (lstat(sFile.c_str(), &st) != 0)
        {
            LOG->error() << __FUNCTION__ << "|" << sFile << "|lstat file error:" << strerror(errno) << endl;
            return -1;
        }
        
        if ((size_t)st.st_size > _MemMax || (size_t)st.st_size < _MemMin)
        {
            LOG->debug() << __FUNCTION__ << "|" << sFile << "|invalid file size:" << _MemMax << "," << _MemMin << "," << st.st_size << endl;
            return -1;
        }

        TC_ThreadLock::Lock lock(_mutex);

        //查看是否已经加载入内存
        std::map<std::string, struct MemState *>::iterator it = _mapFiles.find(sFile);
        if (it != _mapFiles.end())
        {
            cur = it->second;
            
            if (cur->MemLoad  == EM_LOADING || cur->MemLoad == EM_RELOAD)
            {
                LOG->debug() << __FUNCTION__ << "|" << sFile << "|loading file now|" << cur->MemLoad << endl;
                return -1;
            }
            
            if (cur->FileSize == (size_t)st.st_size && cur->FileTime == st.st_mtime && cur->FileInode == st.st_ino)
            {
                mem.first       = cur->MemBuf;
                mem.second      = cur->FileSize;
                cur->MemTime    = time(NULL);
                cur->MemCount++;

                LOG->debug() << __FUNCTION__ << "|" << sFile << "|reuse file in mem" << endl;
                return 0;
            }
            else
            {
                cur->FileSize   = st.st_size;
                cur->FileInode  = st.st_ino;
                cur->FileTime   = st.st_mtime;
                cur->MemLoad    = EM_RELOAD;
                
                mem.first       = cur->MemBuf;
                mem.second      = st.st_size;
                
                LOG->debug() << __FUNCTION__ << "|" << sFile << "|need reload file" << endl;
            }
        }
        else
        {
            if (__getMem(sFile, cur) == -1)
            {
                LOG->error() << __FUNCTION__ << "|" << sFile << "|get free mem fault" << endl;
                return -1;
            }

            cur->FileName   = sFile;
            cur->FileSize   = st.st_size;
            cur->FileInode  = st.st_ino;
            cur->FileTime   = st.st_mtime;

            mem.first       = cur->MemBuf;
            mem.second      = cur->FileSize;  
        }
    }
    catch (std::exception & ex)
    {
        LOG->error() << __FUNCTION__ << "|" << sFile << "|Exception:" << ex.what() << endl;
        return -1;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << sFile << "|Unknown Exception" << endl;
        return -1;
    }
    
    if (__loadFile(sFile, cur->MemBuf, cur->FileSize) == 0)
    {
        cur->MemCount++;
        cur->MemTime = time(NULL);
        cur->MemLoad = EM_LOADED;
        return 0;
    }
    else
    {
        cur->MemTime = 0;
        cur->MemLoad = EM_NLOAD;
        return -1;
    }

    return -1;
}

int PatchCache::release(const std::string & sFile)
{
    LOG->debug() << __FUNCTION__ << "|" << sFile << endl;

    try
    {
        TC_ThreadLock::Lock lock(_mutex);
        
        std::map<std::string, MemState *>::iterator it = _mapFiles.find(sFile);
        if (it == _mapFiles.end())
        {
            LOG->error() << __FUNCTION__ << "|Find '" << sFile << "' fault" << endl;
            return -1;
        }

        it->second->MemCount--;
        
        LOG->debug() << __FUNCTION__ << "|" << sFile << "|N:" << it->second->MemCount << endl;
        return 0;
    }
    catch (std::exception & ex)
    {
        LOG->error() << __FUNCTION__ << "|" << sFile << "|Exception:" << ex.what() << endl;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << sFile << "|Unknown Exception" << endl;
    }
    
    return -1;
}

int PatchCache::__getMem(const std::string & sFile, struct MemState *& cur)
{
    LOG->debug() << __FUNCTION__ << "|" << sFile << endl;
    cur = NULL;
    
    for (size_t i = 0; i < _vecMems.size(); i++)
    {
        struct MemState * st = _vecMems[i];
        
        if ((st->MemLoad == EM_NLOAD || st->MemLoad == EM_LOADED) && st->MemCount == 0 && st->MemTime + 30 <= time(NULL))
        {
            _mapFiles.erase(_vecMems[i]->FileName);
            cur = _vecMems[i];
            break;
        }
    }
    
    if (cur == NULL && _vecMems.size() < _MemNum)
    {
        cur = new MemState();
        _vecMems.push_back(cur);
        _mapFiles.insert(std::make_pair(sFile, cur));
    }

    if (cur != NULL)
    {
        cur->MemCount   = 0;
        cur->MemBuf     = (char *)malloc(_MemMax);
        cur->MemSize    = _MemMax;
        cur->MemLoad    = EM_LOADING;
        return 0;
    }

    LOG->debug() << __FUNCTION__ << "|not enough mem" << endl;
    return -1;
}


int PatchCache::__loadFile(const std::string & sFile, char * szBuff, size_t sizeLen)
{
    LOG->debug() << __FUNCTION__ << "|F:" << sFile << "|M:" << sizeLen << endl;

    int fd = open(sFile.c_str(), O_RDONLY);
    if (fd == -1)
    {
        LOG->error() << __FUNCTION__ << "|F:" << sFile << "|open file error:" << strerror(errno) << endl;
        return -1;
    }

    for (size_t pos = 0; 1; )
    {
        ssize_t r = read(fd, szBuff + pos, sizeLen - pos); 
        if (r > 0)
        {
            pos += r;
            continue;
        }
        
        if (r == 0 || pos >= sizeLen)
        {
            break;
        }

        if (r == -1)
        {
            LOG->error() << __FUNCTION__ << "|F:" << sFile << "|read file error:" << strerror(errno) << endl;

            close(fd);
            return -1;
        }
    }
    
    close(fd);
    return 0;
}

