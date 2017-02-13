#ifndef _PostServer_H_
#define _PostServer_H_

#include <iostream>
#include "servant/Application.h"
#include "Keyword.h"

using namespace taf;

/**
 *
 **/
class PostServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~PostServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

        int getSenstiveLevel()
        {
            return senstiveLevel;
        }
        int getBlackListLevel()
        {
            return blackListLevel;
        }
        Comm::KeywordPrx getKeywordPrx()
        {
            return _pKeywordPrx;
        }
        bool spanCheck(taf::Int64 lUId,const std::string &content);
private:
        taf::TC_ThreadLock spanLock;
        std::map<taf::Int64,pair<std::string,int> > mpSpanCache;
        Comm::KeywordPrx _pKeywordPrx;
        int senstiveLevel;
        int blackListLevel;
};

extern PostServer g_app;

////////////////////////////////////////////
#endif
