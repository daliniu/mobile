//
//  MttPageConfigReq+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-3.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttPageConfigReq+Adapter.h"
#import "MttHDWupAdapters.h"
#import <WirelessUnifiedProtocol/MttPageBrowserReq.h>
#import <WirelessUnifiedProtocol/MttSearchReq.h>
#import <WirelessUnifiedProtocol/MttStaticConfigReq.h>
#import "MttHDPageConfigInfo.h"

#pragma mark - MttPageBrowserReq

@interface MttPageBrowserReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttPageBrowserReq (adapter)

+ (id)mtthdInstance
{
    MttPageBrowserReq *instance = [MttPageBrowserReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    
    instance.jce_version = MttBrowserVersion_NEWWAP2;
    instance.jce_size = 26;
    instance.jce_lastVersionMD5 = pageConfigInfo.md5StringOfStartPage;
    instance.jce_special = MTTSpecialValue_ONELINK;
    return instance;
}

@end

#pragma mark - MttSearchReq

@interface MttSearchReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttSearchReq (adapter)

+ (id)mtthdInstance
{
    MttSearchReq *instance = [MttSearchReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    
    instance.jce_version = 2;
    instance.jce_iSearchTime = pageConfigInfo.searchLastUpdateTime;
    return instance;
}

@end

#pragma mark - MttStaticConfigReq

@interface MttStaticConfigReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttStaticConfigReq (adapter)

+ (id)mtthdInstance
{
    MttStaticConfigReq *instance = [MttStaticConfigReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    instance.jce_iKeywordTime = pageConfigInfo.keywordLastUpdateTime;
    instance.jce_iDomainTime = pageConfigInfo.domainLastUpdateTime;
    instance.jce_iPluginTime = pageConfigInfo.pluginLastUpdateTime;
    instance.jce_iConfTime = pageConfigInfo.configLastUpdateTime;
    return instance;
}

@end

#pragma mark - MttPageConfigReq

@implementation MttPageConfigReq (adapter)

+ (id)mtthdInstance
{
    MttPageConfigReq *instance = [MttPageConfigReq object];
    instance.jce_stUserBase = [MttUserBase mtthdInstance];
    instance.jce_stBrowserReq = [MttPageBrowserReq mtthdInstance];
    instance.jce_stSearchReq = [MttSearchReq mtthdInstance];
    instance.jce_stStaticConfigReq = [MttStaticConfigReq mtthdInstance];
    return instance;
}

@end
