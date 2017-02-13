//
//  MttSTStat+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-10.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttSTStat+Adapter.h"
#import <WirelessUnifiedProtocol/MttLoginRsp.h>
#import "MttHDWupAdapters.h"
#import "MttHDStatInfo.h"

@implementation MttSTTime (Adapter)

+ (id)mtthdInstance
{
    MttSTTime *instance = [MttSTTime object];
    instance.jce_iAvgProxyTime = 0;
    instance.jce_iAvgDirectTime = 0;
    return instance;
}

@end

@implementation MttSTStat (Adapter)

+ (id)mtthdInstance
{
    MttSTStat *instance = [MttSTStat object];
    MttHDStatInfo *statInfo = [MttHDStatInfo theSpecial];
    instance.jce_stUB = [MttUserBase mtthdInstance];
    instance.jce_stTime = [MttSTTime mtthdInstance];
    instance.jce_stTotal = [statInfo totalPV];
    instance.jce_stOuterPv = statInfo.outerPV;
    instance.jce_stInnerPv = statInfo.innerPV;
    instance.jce_iUseTime = statInfo.useTime;
    instance.jce_vEntryPv = statInfo.entryPV;
    instance.jce_sProtocol = statInfo.protocol;
    return instance;
}

+ (void)mtthdInstanceReset
{
    MttHDStatInfo* oldStatInfo = [MttHDStatInfo clearTheSpecial];
    MttHDStatInfo *statInfo = [MttHDStatInfo theSpecial];
    [statInfo setMaximumTabs:oldStatInfo.maximumTabs];
}

@end
